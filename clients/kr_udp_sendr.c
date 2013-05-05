#include <libswscale/swscale.h>

#include "kr_client.h"
#include "krad_udp.h"
#include "krad_vpx.h"
#include "krad_v4l2.h"

typedef struct kr_udp_sendr_St kr_udp_sendr_t;

struct kr_udp_sendr_St {
  uint32_t width;
  uint32_t height;
  uint32_t frames;
  krad_udp_t *udp;
	krad_vpx_encoder_t *vpxenc;	
	krad_v4l2_t *krad_v4l2;
	int bitrate;
	char device[256];
};

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

void kr_udp_sendr (kr_udp_sendr_t *udp_sendr, char *host, int port) {

	int keyframe;
	void *captured_frame;
  struct SwsContext *scaler;
  int packet_size;
  unsigned char *planes[3];
  int strides[4];
  unsigned char *oplanes[3];
  int ostrides[4];
  void *video_packet;

	scaler = NULL;

	udp_sendr->udp = kr_udp (host, port, "", "");

  udp_sendr->krad_v4l2 = krad_v4l2_create ();
  udp_sendr->vpxenc = krad_vpx_encoder_create (udp_sendr->width,
                                               udp_sendr->height,
                                               30, 1, udp_sendr->bitrate);

  krad_v4l2_open (udp_sendr->krad_v4l2, udp_sendr->device,
                  udp_sendr->width, udp_sendr->height, 30);

  udp_sendr->vpxenc->cfg.kf_min_dist = 10;
  udp_sendr->vpxenc->cfg.kf_max_dist = 90;

  krad_vpx_encoder_config_set (udp_sendr->vpxenc, &udp_sendr->vpxenc->cfg);

  krad_vpx_encoder_deadline_set (udp_sendr->vpxenc, 1);
  krad_vpx_encoder_print_config (udp_sendr->vpxenc);
  
  krad_v4l2_start_capturing (udp_sendr->krad_v4l2);

	while (1) {

	  if (destroy == 1) {
		  printf ("Got signal!\n");
	    break;
	  }

    captured_frame = krad_v4l2_read (udp_sendr->krad_v4l2);    

    if (captured_frame != NULL) {
      
      planes[0] = captured_frame;
      planes[1] = NULL;
      planes[2] = NULL;
      strides[0] = udp_sendr->width + (udp_sendr->width/2) * 2;
      strides[1] = 0;
      strides[2] = 0;
      strides[3] = 0;
     
      oplanes[0] = udp_sendr->vpxenc->image->planes[0];
      oplanes[1] = udp_sendr->vpxenc->image->planes[1];
      oplanes[2] = udp_sendr->vpxenc->image->planes[2];
      ostrides[0] = udp_sendr->vpxenc->image->stride[0];
      ostrides[1] = udp_sendr->vpxenc->image->stride[1];
      ostrides[2] = udp_sendr->vpxenc->image->stride[2]; 
     
      scaler = sws_getCachedContext ( scaler,
                                      udp_sendr->width,
                                      udp_sendr->height,
                                      PIX_FMT_YUYV422,
                                      udp_sendr->width,
                                      udp_sendr->height,
                                      PIX_FMT_YUV420P, 
                                      SWS_BICUBIC,
                                      NULL, NULL, NULL);
      
       sws_scale (scaler, (const uint8_t * const*)planes, strides,
                  0, udp_sendr->height, oplanes, ostrides);

       krad_v4l2_frame_done (udp_sendr->krad_v4l2);
      
       packet_size = krad_vpx_encoder_write (udp_sendr->vpxenc,
                                            (unsigned char **)&video_packet,
                                            &keyframe);

       if (packet_size > 0) {
         krad_slicer_sendto (udp_sendr->udp->slicer,
                             video_packet, packet_size,
                             1, keyframe,
                             udp_sendr->udp->host, udp_sendr->udp->port);

         udp_sendr->frames++;
       }
	  }
	}

  krad_vpx_encoder_destroy (&udp_sendr->vpxenc);
  krad_v4l2_stop_capturing (udp_sendr->krad_v4l2);
  krad_v4l2_close(udp_sendr->krad_v4l2);
  krad_v4l2_destroy(udp_sendr->krad_v4l2);
  sws_freeContext ( scaler );
  kr_udp_destroy (&udp_sendr->udp);
}

int main (int argc, char *argv[]) {

  kr_udp_sendr_t *udp_sendr;
  int port;
  
  port = 3777;

	if (argc < 2) {
    printf ("use: host port width height bitrate device\n");
		return 1;
	}
	
	krad_system_init ();
	
	char logfile[265];
  sprintf (logfile, "%s/kr_udp_sendr_%"PRIu64".log",
           getenv ("HOME"), krad_unixtime ());	
	krad_system_log_on (logfile);

  udp_sendr = calloc (1, sizeof (kr_udp_sendr_t));

  udp_sendr->bitrate = 800;
  
	if (argc > 2) {
	  port = atoi (argv[2]);
	}
	
  udp_sendr->width = 640;
  udp_sendr->height = 360;
  
	if (argc >= 5) {
    udp_sendr->width = atoi(argv[3]);
    udp_sendr->height = atoi(argv[4]);
  }
  
	if (argc >= 6) {
    udp_sendr->bitrate = atoi(argv[5]);
  }
  
	if (argc >= 7) {
    strncpy (udp_sendr->device, argv[6], sizeof(udp_sendr->device));
  } else {
    strncpy (udp_sendr->device, "/dev/video0", sizeof(udp_sendr->device));
  }
  
	signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);	
	
  kr_udp_sendr (udp_sendr, argv[1], port);	

	if (udp_sendr->frames > 0) {
		printf ("Sent %d frames!\n", udp_sendr->frames);
	}

  free (udp_sendr);

	return 0;	
}
