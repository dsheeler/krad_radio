#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <stdint.h>
#include <inttypes.h>

#include <libiz.h>
#include "krad_vhs.h"

#ifdef __cplusplus
extern "C" {
#include <libswscale/swscale.h>
}
#endif


KrImage::KrImage()
    : m_components(0)
{
}

KrImage::~KrImage()
{
}

#ifdef __cplusplus
extern "C" {
#endif

int save_file (void *buffer, unsigned int size) {
	
	static int comped = 0;
	char snapname[128];
	FILE *fp;
	
	sprintf (snapname, "/home/oneman/kode/comped_%d.iz", comped++);

  fp = fopen (snapname, "wb");
  fwrite (buffer, 1, size, fp);
  fclose (fp);

	return 0;

}

int open_file (void *buffer) {
	
	static int comped = 0;
	char snapname[128];
	FILE *fp;
	int ret;
	
	sprintf (snapname, "/home/oneman/kode/comped_%d.iz", comped++);

  fp = fopen (snapname, "rb");
  ret = fread (buffer, 1, 1280 * 720 * 3, fp);
  fclose (fp);

	return ret;

}

void decode_iz (krad_vhs_t *krad_vhs, unsigned char *src, unsigned char *dst) {

    KrImage kimage;

    kimage.setWidth(krad_vhs->width);
    kimage.setHeight(krad_vhs->height);
    kimage.setSamplesPerLine(krad_vhs->width * 3);
    kimage.setData(dst);        
    
    IZ::decodeImageSize(kimage, src);
    IZ::decodeImage(kimage, src);

}

size_t encode_iz (krad_vhs_t *krad_vhs, unsigned char *src, unsigned char *dst) {

    unsigned char *end;    
    KrImage kimage;
    
    kimage.setWidth(krad_vhs->width);
    kimage.setHeight(krad_vhs->height);
    kimage.setSamplesPerLine(krad_vhs->width * 3);
    kimage.setData(src);        
    
    end = IZ::encodeImage(kimage, dst);
    
    return end - dst;

}

void init_iz () {
  IZ::initEncodeTable();
  IZ::initDecodeTable();
}




  
int krad_vhs_encode (krad_vhs_t *krad_vhs, unsigned char *pixels) {

  int encoded_size;
	int input_rgb_stride_arr[4];
	int output_rgb_stride_arr[4];
	unsigned char *dsty[4];
	unsigned char *srcy[4];
	
  input_rgb_stride_arr[1] = 0;
  output_rgb_stride_arr[1] = 0;	
  input_rgb_stride_arr[2] = 0;
  output_rgb_stride_arr[2] = 0;	
  input_rgb_stride_arr[3] = 0;
  output_rgb_stride_arr[3] = 0;	  	

  srcy[0] = pixels;
  dsty[0] = krad_vhs->buffer;
          
  input_rgb_stride_arr[0] = 4*krad_vhs->width;
  output_rgb_stride_arr[0] = 3*krad_vhs->width;
  
          
  sws_scale (krad_vhs->converter, (const uint8_t * const*)srcy,
				     input_rgb_stride_arr, 0, krad_vhs->height, dsty, output_rgb_stride_arr);
          
  encoded_size = encode_iz (krad_vhs, krad_vhs->buffer, krad_vhs->enc_buffer);
  
  return encoded_size;

}


int krad_vhs_decode (krad_vhs_t *krad_vhs, unsigned char *buffer, unsigned char *pixels) {

	int input_rgb_stride_arr[4];
	int output_rgb_stride_arr[4];
	unsigned char *dsty[4];
	unsigned char *srcy[4];
	
  input_rgb_stride_arr[1] = 0;
  output_rgb_stride_arr[1] = 0;	
  input_rgb_stride_arr[2] = 0;
  output_rgb_stride_arr[2] = 0;	
  input_rgb_stride_arr[3] = 0;
  output_rgb_stride_arr[3] = 0;	  	

  srcy[0] = krad_vhs->buffer;
  dsty[0] = pixels;
      
  input_rgb_stride_arr[0] = 3*krad_vhs->width;
  output_rgb_stride_arr[0] = 4*krad_vhs->width;    

        
  decode_iz (krad_vhs, buffer, krad_vhs->buffer);

                           
  sws_scale (krad_vhs->converter, (const uint8_t * const*)srcy,
             input_rgb_stride_arr, 0, krad_vhs->height, dsty, output_rgb_stride_arr);          

  return 0;

}


void krad_vhs_destroy (krad_vhs_t *krad_vhs) {

	if (krad_vhs->converter != NULL) {
		sws_freeContext ( krad_vhs->converter );
		krad_vhs->converter = NULL;
	}

	if (krad_vhs->buffer != NULL) {
		free ( krad_vhs->buffer );
		krad_vhs->buffer = NULL;
	}
	

	if (krad_vhs->enc_buffer != NULL) {
		free ( krad_vhs->enc_buffer );
		krad_vhs->enc_buffer = NULL;
	}

	free (krad_vhs);

}

krad_vhs_t *krad_vhs_create_decoder () {

	krad_vhs_t *krad_vhs = (krad_vhs_t *)calloc(1, sizeof(krad_vhs_t));
	
  init_iz (); //fixme
	
  krad_vhs->buffer = (unsigned char *)malloc (1920 * 1080 * 4);	
	
  krad_vhs->converter =
				sws_getContext ( krad_vhs->width,
								 krad_vhs->height,
								 PIX_FMT_BGR24,
								 krad_vhs->width,
								 krad_vhs->height,
								 PIX_FMT_RGB32, 
								 SWS_BICUBIC,
								 NULL, NULL, NULL);	
	
	return krad_vhs;

}

krad_vhs_t *krad_vhs_create_encoder (int width, int height) {

	krad_vhs_t *krad_vhs = (krad_vhs_t *)calloc(1, sizeof(krad_vhs_t));
	
  krad_vhs->buffer = (unsigned char *)malloc (1920 * 1080 * 4);
  krad_vhs->enc_buffer = (unsigned char *)malloc (1920 * 1080 * 4);

  init_iz ();//fixme
	
	krad_vhs->encoder = 1;
	krad_vhs->width = width;
	krad_vhs->height = height;		
	
  krad_vhs->converter =
				sws_getContext ( krad_vhs->width,
								 krad_vhs->height,
								 PIX_FMT_RGB32,
								 krad_vhs->width,
								 krad_vhs->height,
								 PIX_FMT_BGR24, 
								 SWS_BICUBIC,
								 NULL, NULL, NULL);	
	
	return krad_vhs;

}

#ifdef __cplusplus
}
#endif
