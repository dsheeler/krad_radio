#include <cairo/cairo.h>
#include <libswscale/swscale.h>

#include "kr_client.h"
#include "krad_udp.h"
#include "krad_vpx.h"
#include "krad_wayland.h"

#define GREY  0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255   * 1.0
#define BLUE 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0
#define BGCOLOR_CLR  0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255   * 1.0, 0.255 / 0.255   * 1.0
#define ORANGE  0.255 / 0.255 * 1.0, 0.080 / 0.255 * 1.0, 0.0

#define DEFAULT_FBUFSIZE 120

typedef struct kr_udp_recvr_St kr_udp_recvr_t;

struct kr_udp_recvr_St {
  uint32_t width;
  uint32_t height;
	kr_wayland *wayland;
	krad_vpx_decoder_t *vpxdec;
	
  uint32_t frame_size;
  uint32_t framebufsize;
  unsigned char *rgba;
  uint32_t frames_dec;
  uint32_t consumed;
	
	uint32_t repeat;
	void *buffer;  
};

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

void render_hex (cairo_t *cr, int x, int y, int w) {

	cairo_pattern_t *pat;
	static float hexrot = 0;
	int r1;
	float scale;
		
	cairo_save(cr);
	cairo_set_line_width(cr, 1);
	cairo_set_source_rgb(cr, ORANGE);

	scale = 2.5;
	r1 = ((w)/2 * sqrt(3));

	cairo_translate (cr, x, y);
	cairo_rotate (cr, hexrot * (M_PI/180.0));
	cairo_translate (cr, -(w/2), -r1);

	cairo_move_to (cr, 0, 0);
	cairo_rel_line_to (cr, w, 0);
	cairo_rotate (cr, 60 * (M_PI/180.0));
	cairo_rel_line_to (cr, w, 0);
	cairo_rotate (cr, 60 * (M_PI/180.0));
	cairo_rel_line_to (cr, w, 0);
	cairo_rotate (cr, 60 * (M_PI/180.0));
	cairo_rel_line_to (cr, w, 0);
	cairo_rotate (cr, 60 * (M_PI/180.0));
	cairo_rel_line_to (cr, w, 0);
	cairo_rotate (cr, 60 * (M_PI/180.0));
	cairo_rel_line_to (cr, w, 0);
	hexrot += 1.5;
	cairo_fill (cr);
	
	cairo_restore(cr);
	cairo_save(cr);
		
	cairo_set_line_width(cr, 1.5);
	cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
	cairo_set_source_rgb(cr, GREY);


	cairo_translate (cr, x, y);
	cairo_rotate (cr, hexrot * (M_PI/180.0));
	cairo_translate (cr, -((w * scale)/2), -r1 * scale);
	cairo_scale(cr, scale, scale);

	cairo_move_to (cr, 0, 0);
	cairo_rel_line_to (cr, w, 0);
	cairo_rotate (cr, 60 * (M_PI/180.0));
	cairo_rel_line_to (cr, w, 0);
	cairo_rotate (cr, 60 * (M_PI/180.0));
	cairo_rel_line_to (cr, w, 0);
	cairo_rotate (cr, 60 * (M_PI/180.0));
	cairo_rel_line_to (cr, w, 0);
	cairo_rotate (cr, 60 * (M_PI/180.0));
	cairo_rel_line_to (cr, w, 0);
	cairo_rotate (cr, 60 * (M_PI/180.0));
	cairo_rel_line_to (cr, w, 0);
	
	cairo_rotate (cr, 60 * (M_PI/180.0));

	cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
	pat = cairo_pattern_create_radial (w/2, r1, 3, w/2, r1, r1*scale);
	cairo_pattern_add_color_stop_rgba (pat, 0, 0, 0, 1, 1);
	cairo_pattern_add_color_stop_rgba (pat, 0.4, 0, 0, 0, 0);
	cairo_set_source (cr, pat);
	
	cairo_fill (cr);
	cairo_pattern_destroy (pat);
	cairo_restore(cr);

}

void kr_udp_recvr_free_framebuf (kr_udp_recvr_t *udp_recvr) {
  if (udp_recvr->rgba != NULL) {
    free (udp_recvr->rgba);
    udp_recvr->rgba = NULL;
  }
}

void kr_udp_recvr_alloc_framebuf (kr_udp_recvr_t *udp_recvr) {
  udp_recvr->framebufsize = DEFAULT_FBUFSIZE;
  udp_recvr->frame_size = udp_recvr->width * udp_recvr->height * 4;
  udp_recvr->rgba = malloc (udp_recvr->frame_size * udp_recvr->framebufsize);
}

int udp_recvr_frame (void *pointer, uint32_t time) {

	int updated;
  int frame;
  kr_udp_recvr_t *udp_recvr;
  int pos;
  
	updated = 0;
	
  udp_recvr = (kr_udp_recvr_t *)pointer;
	
  frame = udp_recvr->frames_dec - 1;

  while (udp_recvr->consumed >= frame) {
    frame++;
  }
  
  if (frame > udp_recvr->frames_dec) {
    frame = udp_recvr->frames_dec;
  }

  if (udp_recvr->consumed != frame) {
    updated = 1;
  }

  udp_recvr->consumed = frame;

  pos = (frame % udp_recvr->framebufsize);

  memcpy (udp_recvr->buffer,
          udp_recvr->rgba + (pos * udp_recvr->frame_size),
          udp_recvr->width * udp_recvr->height * 4);


	return updated;
}


void kr_udp_recvr (kr_udp_recvr_t *udp_recvr, int port) {

	krad_rebuilder_t *krad_rebuilder;
	int sd;
	int keyframe;
	int started;
	int ret;
	int slen;
	unsigned char *buffer;
	unsigned char *packet_buffer;
	struct sockaddr_in local_address;
	struct sockaddr_in remote_address;
  struct SwsContext *scaler;
  struct pollfd pollfds[4];

	scaler = NULL;
	started = 0;
	slen = sizeof (remote_address);
	
	buffer = calloc (1, 45000);
	packet_buffer = calloc (1, 2300000);
	sd = socket (AF_INET, SOCK_DGRAM, 0);
	krad_rebuilder = krad_rebuilder_create ();

	memset((char *) &local_address, 0, sizeof(local_address));
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons (port);
	local_address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind (sd, (struct sockaddr *)&local_address, sizeof(local_address)) == -1 ) {
		printf("bind error\n");
		exit(1);
	}
	
  kr_udp_recvr_alloc_framebuf (udp_recvr);
	
  udp_recvr->vpxdec = krad_vpx_decoder_create ();

	while (1) {
	
	  if (destroy == 1) {
		  printf ("Got signal!\n");
	    break;
	  }
	  
    pollfds[0].fd = kr_wayland_get_fd(udp_recvr->wayland);
    pollfds[0].events = POLLIN;

    pollfds[1].fd = sd;
    pollfds[1].events = POLLIN;
	  
    ret = poll (pollfds, 2, 1);
	  
	  if (ret < 0) {
	    break;
	  }
	  
    if (pollfds[0].revents == POLLIN) { 
      kr_wayland_process(udp_recvr->wayland);
    }
	
    if (pollfds[1].revents == POLLIN) {
		  ret = recvfrom (sd, buffer, 2000, 0, (struct sockaddr *)&remote_address, (socklen_t *)&slen);
		
		  if (ret == -1) {
			  printf("failed recvin udp\n");
			  exit (1);
		  }
		
      //printf ("Received packet from %s:%d\n", 
    	//	inet_ntoa(remote_address.sin_addr), ntohs(remote_address.sin_port));


		  krad_rebuilder_write (krad_rebuilder, buffer, ret);
		  ret = krad_rebuilder_read_packet (krad_rebuilder, packet_buffer, 1, &keyframe);
    
      if (ret != 0) {

		    //printf ("read a packet with %d bytes key: %d     \n", ret, keyframe);

        if ((started == 1) || ((started == 0) && (keyframe == 1))) {
		      started = 1;
			  } else {
			    continue;
			  }
			
        krad_vpx_decoder_decode (udp_recvr->vpxdec, packet_buffer, ret);
        
        while (udp_recvr->vpxdec->img != NULL) {
       
            int rgb_stride_arr[3] = {4*udp_recvr->width, 0, 0};
            uint8_t *dst[4];
            
            scaler = sws_getCachedContext ( scaler,
                                            udp_recvr->vpxdec->width,
                                            udp_recvr->vpxdec->height,
                                            PIX_FMT_YUV420P,
                                            udp_recvr->width,
                                            udp_recvr->height,
                                            PIX_FMT_RGB32, 
                                            SWS_BICUBIC,
                                            NULL, NULL, NULL);


          int pos = ((udp_recvr->frames_dec + 1) % udp_recvr->framebufsize) * udp_recvr->frame_size;
          dst[0] = (unsigned char *)udp_recvr->rgba + pos;

          sws_scale (scaler,
                     (const uint8_t * const*)udp_recvr->vpxdec->img->planes,
                      udp_recvr->vpxdec->img->stride,
                     0, udp_recvr->vpxdec->height,
                     dst, rgb_stride_arr);


          udp_recvr->frames_dec++;
            //printf ("Received frame!\n");
          krad_vpx_decoder_decode_again (udp_recvr->vpxdec);
            
		    }
	    }
	  }
	}

  krad_vpx_decoder_destroy (&udp_recvr->vpxdec);

  kr_udp_recvr_free_framebuf (udp_recvr);

	krad_rebuilder_destroy (krad_rebuilder);
	close (sd);
  sws_freeContext ( scaler );
	free (packet_buffer);
	free (buffer);
}

int main (int argc, char *argv[]) {

  kr_udp_recvr_t *udp_recvr;
  int port;
  
  printf ("Options: port width height\n");

  udp_recvr = calloc (1, sizeof (kr_udp_recvr_t));

  port = 3777;  
  udp_recvr->width = 960;
  udp_recvr->height = 540;

	if (argc > 1) {
	  port = atoi (argv[1]);
	}
	
	if (argc > 3) {
    udp_recvr->width = atoi (argv[2]);
    udp_recvr->height = atoi (argv[3]);
	}

	signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);	

  kr_udp_recvr_alloc_framebuf (udp_recvr);

	udp_recvr->wayland = kr_wayland_create();

	kr_wayland_window_create(udp_recvr->wayland, udp_recvr->width,
   udp_recvr->height, &udp_recvr->buffer);

  kr_wayland_set_frame_callback (udp_recvr->wayland, udp_recvr_frame, udp_recvr);

  printk("Wayland display prepared");

  kr_wayland_open_window(udp_recvr->wayland);
	
  kr_udp_recvr (udp_recvr, port);	

	if (udp_recvr->frames_dec > 0) {
		printf ("Rendered %d frames!\n", udp_recvr->frames_dec);
	}

  kr_wayland_close_window (udp_recvr->wayland);
	kr_wayland_destroy (udp_recvr->wayland);

  kr_udp_recvr_free_framebuf (udp_recvr);

  free (udp_recvr);

	return 0;	
}
