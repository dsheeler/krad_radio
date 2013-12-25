#include <cairo/cairo.h>
#include <libswscale/swscale.h>

#include "kr_client.h"
#include "krad_udp.h"
#include "krad_vpx.h"
#include "krad_opus.h"
#include "krad_ring.h"
#include "krad_resample_ring.h"

#define GREY  0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255   * 1.0
#define BLUE 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0
#define BGCOLOR_CLR  0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255   * 1.0, 0.255 / 0.255   * 1.0
#define ORANGE  0.255 / 0.255 * 1.0, 0.080 / 0.255 * 1.0, 0.0

#define DEFAULT_FBUFSIZE 120

typedef struct kr_udp_recvr_St kr_udp_recvr_t;

struct kr_udp_recvr_St {
  uint32_t width;
  uint32_t height;
  uint32_t frames;
  kr_client_t *client;
  kr_videoport_t *videoport;
  krad_vpx_decoder_t *vpxdec;
  krad_opus_t *krad_opus;
  kr_audioport_t *audioport;
  int direction;
  uint32_t sample_rate;

  uint32_t frame_size;
  uint32_t framebufsize;
  unsigned char *rgba;
  uint32_t frames_dec;
  uint32_t consumed;

  uint32_t repeat;
  float *audio;
  krad_ringbuffer_t *ringbuffer[2];
  krad_resample_ring_t *resample_ring[2];
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

int videoport_process (void *buffer, void *user) {

  kr_udp_recvr_t *udp_recvr;
  int pos;

  udp_recvr = (kr_udp_recvr_t *)user;

  /*

  cairo_surface_t *cst;
  cairo_t *cr;

  cst = cairo_image_surface_create_for_data ((unsigned char *)buffer,
                         CAIRO_FORMAT_ARGB32,
                         udp_recvr->width,
                         udp_recvr->height,
                         udp_recvr->width * 4);

  cr = cairo_create (cst);
  cairo_save (cr);
  cairo_set_source_rgba (cr, BGCOLOR_CLR);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);
  cairo_restore (cr);
  //render_hex (cr, udp_recvr->width / 2, udp_recvr->height / 2, 66);
  cairo_surface_flush (cst);
  cairo_destroy (cr);
  cairo_surface_destroy (cst);
*/

  int frame;

  frame = udp_recvr->frames_dec - 1;

  while (udp_recvr->consumed >= frame) {
    frame++;
  }

  if (frame > udp_recvr->frames_dec) {
    frame = udp_recvr->frames_dec;
  }

  udp_recvr->consumed = frame;

  pos = (frame % udp_recvr->framebufsize);

  memcpy (buffer,
          udp_recvr->rgba + (pos * udp_recvr->frame_size),
          udp_recvr->width * udp_recvr->height * 4);

  return 0;
}

int audioport_process (uint32_t nframes, void *arg) {

  float *buffer[2];
  kr_udp_recvr_t *udp_recvr = (kr_udp_recvr_t *)arg;


  buffer[0] = kr_audioport_get_buffer (udp_recvr->audioport, 0);
  buffer[1] = kr_audioport_get_buffer (udp_recvr->audioport, 1);

  if (udp_recvr->sample_rate != 48000) {
    if ((krad_resample_ring_read_space (udp_recvr->resample_ring[0]) >= nframes * 4) &&
       (krad_resample_ring_read_space (udp_recvr->resample_ring[1]) >= nframes * 4)) {
         krad_resample_ring_read (udp_recvr->resample_ring[0], (unsigned char *)buffer[0], nframes * 4);
        krad_resample_ring_read (udp_recvr->resample_ring[1], (unsigned char *)buffer[1], nframes * 4);
    } else {
      memset(buffer[0], '0', nframes * 4);
      memset(buffer[1], '0', nframes * 4);
    }
  } else {
    if ((krad_ringbuffer_read_space (udp_recvr->ringbuffer[0]) >= nframes * 4) &&
       (krad_ringbuffer_read_space (udp_recvr->ringbuffer[1]) >= nframes * 4)) {
         krad_ringbuffer_read (udp_recvr->ringbuffer[0], (char *)buffer[0], nframes * 4);
        krad_ringbuffer_read (udp_recvr->ringbuffer[1], (char *)buffer[1], nframes * 4);
    } else {
      memset(buffer[0], '0', nframes * 4);
      memset(buffer[1], '0', nframes * 4);
    }
  }
  return 0;
}

void kr_udp_recvr (kr_udp_recvr_t *udp_recvr, int port) {

  krad_rebuilder_t *krad_rebuilder;
  int sd;
  int keyframe;
  int c;
  int bytes;
  int started;
  int ret;
  int slen;
  unsigned char *buffer;
  unsigned char *packet_buffer;
  struct sockaddr_in local_address;
  struct sockaddr_in remote_address;
  struct SwsContext *scaler;

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


  udp_recvr->vpxdec = krad_vpx_decoder_create ();

  while (1) {

    if (destroy == 1) {
      printf ("Got signal!\n");
      break;
    }

    if (kr_audioport_error (udp_recvr->audioport)) {
      printf ("Error: %s\n", "Audioport Error");
      ret = 1;
      break;
    }

    if (kr_videoport_error (udp_recvr->videoport)) {
      printf ("Error: %s\n", "videoport Error");
      break;
    }

    ret = recvfrom (sd, buffer, 2000, 0, (struct sockaddr *)&remote_address, (socklen_t *)&slen);

    if (ret == -1) {
      printf("failed recvin udp\n");
      exit (1);
    }

    //printf ("Received packet from %s:%d\n",
    //  inet_ntoa(remote_address.sin_addr), ntohs(remote_address.sin_port));


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

        krad_vpx_decoder_decode_again (udp_recvr->vpxdec);
      }
    }

    ret = krad_rebuilder_read_packet (krad_rebuilder, packet_buffer, 2, &keyframe);
    if (ret != 0) {
      krad_opus_decoder_write (udp_recvr->krad_opus, packet_buffer, ret);
      bytes = -1;
      while (bytes != 0) {
        for (c = 0; c < 2; c++) {
          bytes = krad_opus_decoder_read (udp_recvr->krad_opus, c + 1, (char *)udp_recvr->audio, 120 * 4);
          if (bytes) {
            if ((bytes / 4) != 120) {
              failfast ("uh oh crazyto");
            }

            if (udp_recvr->sample_rate != 48000) {
              while (krad_resample_ring_write_space (udp_recvr->resample_ring[c]) < bytes) {
                  usleep(20000);
              }
              //krad_ringbuffer_write (krad_link->audio_output_ringbuffer[c], (char *)audio, bytes);
              krad_resample_ring_write (udp_recvr->resample_ring[c], (unsigned char *)udp_recvr->audio, bytes);
            } else {
              while (krad_ringbuffer_write_space (udp_recvr->ringbuffer[c]) < bytes) {
                  usleep(20000);
              }
              //krad_ringbuffer_write (udp_recvr->ringbuffer[c], (char *)audio, bytes);
              krad_ringbuffer_write (udp_recvr->ringbuffer[c], (char *)udp_recvr->audio, bytes);
            }
          }
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

  int ret;
  int c;
  uint32_t width;
  uint32_t height;
  kr_client_t *client;
  kr_videoport_t *videoport;
  kr_udp_recvr_t *udp_recvr;
  int port;

  port = 3777;
  ret = 0;

  if (argc < 2) {
    if (argc > 2) {
      fprintf (stderr, "Only takes station argument.\n");
    } else {
      fprintf (stderr, "No station specified.\n");
    }
    return 1;
  }

  krad_system_init ();

  char logfile[265];
  sprintf (logfile, "%s/kr_udp_recvro_%"PRIu64".log",
           getenv ("HOME"), krad_unixtime ());
  krad_system_log_on (logfile);

  if (argc > 2) {
    port = atoi (argv[2]);
  }

  client = kr_client_create ("krad videoport client");

  if (client == NULL) {
    fprintf (stderr, "Could not create KR client.\n");
    return 1;
  }

  kr_connect (client, argv[1]);

  if (!kr_connected (client)) {
    fprintf (stderr, "Could not connect to %s krad radio daemon.\n", argv[1]);
    kr_client_destroy (&client);
    return 1;
  }

  if (kr_compositor_get_info_wait (client, &width, &height, NULL, NULL) != 1) {
    kr_client_destroy (&client);
    return 1;
  }


  videoport = kr_videoport_create (client, KR_MXR_INPUT);

  if (videoport == NULL) {
    fprintf (stderr, "Could not make videoport.\n");
    kr_client_destroy (&client);
    return 1;
  } else {
    printf ("Working!\n");
  }

  udp_recvr = calloc (1, sizeof (kr_udp_recvr_t));

  udp_recvr->width = width;
  udp_recvr->height = height;
  kr_udp_recvr_alloc_framebuf (udp_recvr);
  udp_recvr->client = client;
  udp_recvr->videoport = videoport;

  kr_videoport_set_callback (videoport, videoport_process, udp_recvr);

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);

  kr_videoport_activate (videoport);


  if (kr_mixer_get_info_wait (client, &udp_recvr->sample_rate, NULL) != 1) {
    fprintf (stderr, "Could not get mixer info!\n");
    kr_client_destroy (&client);
    return 1;
  }

  udp_recvr->audio = calloc(1, 8192 * 4 * 4);
  for (c = 0; c < 2; c++) {
    udp_recvr->ringbuffer[c] = krad_ringbuffer_create (1000000);

    if (udp_recvr->sample_rate != 48000) {
      udp_recvr->resample_ring[c] = krad_resample_ring_create (2000000, 48000,
                                               udp_recvr->sample_rate);
    }
  }

  krad_opus_t *tmphack;

  tmphack = krad_opus_encoder_create (2, 48000,
                                      128000, OPUS_APPLICATION_AUDIO);

  udp_recvr->krad_opus = krad_opus_decoder_create (&tmphack->krad_codec_header);

  krad_opus_encoder_destroy (tmphack);

  udp_recvr->direction = KR_MXR_INPUT;
  udp_recvr->audioport = kr_audioport_create(client, "udp recvr",
   udp_recvr->direction);

  if (udp_recvr->audioport == NULL) {
    fprintf (stderr, "Could not make audioport.\n");
    kr_client_destroy (&client);
    return 1;
  } else {
    printf ("Working!\n");
  }

  kr_audioport_set_callback (udp_recvr->audioport, audioport_process, udp_recvr);

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);

  kr_audioport_connect(udp_recvr->audioport);

  kr_udp_recvr (udp_recvr, port);


  kr_audioport_disconnect (udp_recvr->audioport);

  kr_audioport_destroy (udp_recvr->audioport);

  kr_videoport_deactivate (videoport);

  kr_videoport_destroy (videoport);
  krad_opus_decoder_destroy(udp_recvr->krad_opus);
  kr_client_destroy (&client);

  if (udp_recvr->frames > 0) {
    printf ("Rendered %d frames!\n", udp_recvr->frames);
  }

  for (c = 0; c < 2; c++) {
    krad_ringbuffer_free ( udp_recvr->ringbuffer[c] );

    if (udp_recvr->sample_rate != 48000) {
      krad_resample_ring_destroy ( udp_recvr->resample_ring[c] );
    }

  }
  free (udp_recvr->audio);
  free (udp_recvr);

  if (ret == 0) {
    printf ("Worked!\n");
  }

  return ret;
}
