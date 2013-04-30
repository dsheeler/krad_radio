#include "krad_fc2.h"

void kr_fc2_set_timestamping (kr_fc2_t *fc, BOOL enableTimeStamp) {

  fc2Error error;
  fc2EmbeddedImageInfo embeddedInfo;

  error = fc2GetEmbeddedImageInfo (fc->context, &embeddedInfo);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2GetEmbeddedImageInfo: %d\n", error);
  }

  if (embeddedInfo.timestamp.available != 0) { 
    embeddedInfo.timestamp.onOff = enableTimeStamp;
  }    

  error = fc2SetEmbeddedImageInfo (fc->context, &embeddedInfo);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2SetEmbeddedImageInfo: %d\n", error);
  }
}

void kr_fc2_capture_image (kr_fc2_t *fc, krad_frame_t *frame) {

  fc2Error error;

  error = fc2RetrieveBuffer (fc->context, &fc->image);
  if (error != FC2_ERROR_OK) {
    printke ("Error in retrieveBuffer: %d", error);
  } else {
    fc2TimeStamp ts = fc2GetImageTimeStamp (&fc->image);
    int diff = (ts.cycleSeconds - fc->prevTimestamp.cycleSeconds) * 8000
                + (ts.cycleCount - fc->prevTimestamp.cycleCount);
    fc->prevTimestamp = ts;
    printk ( 
        "timestamp [%d %d] - %d", 
        ts.cycleSeconds, 
        ts.cycleCount, 
        diff );
  }
  
  fc2ConvertImageTo (FC2_PIXEL_FORMAT_RGB, &fc->image, &fc->image2);
  
  printk ("Frame rows %u cols %u stride %u datasize %u format %d",
          fc->image.rows, fc->image.cols, fc->image.stride,
          fc->image.dataSize, fc->image.format);
  
  frame->width = fc->image2.cols;
  frame->height = fc->image2.rows;
  frame->yuv_strides[0] = fc->image2.stride;
  frame->yuv_pixels[0] = fc->image2.pData;
  //frame->format = AV_PIX_FMT_GRAY8;
  frame->format = AV_PIX_FMT_RGB24;

  frame->yuv_pixels[1] = NULL;
  frame->yuv_pixels[2] = NULL;
  frame->yuv_strides[1] = 0;
  frame->yuv_strides[2] = 0;
  frame->yuv_strides[3] = 0;

  /*
	int *pixels;
	int refs;
	int encoded_size;
	
	int format;
	
	uint8_t *yuv_pixels[4];
	int yuv_strides[4];
	
	cairo_surface_t *cst;
	cairo_t *cr;	
	
	uint64_t timecode;
	
	int width;
	int height;
	*/
}

void kr_fc2_capture_start (kr_fc2_t *fc) {

  fc2Error error;
  fc2VideoMode video_mode;
  fc2FrameRate frame_rate;

  video_mode = FC2_VIDEOMODE_640x480Y8;
  frame_rate = FC2_FRAMERATE_30;
  frame_rate = FC2_FRAMERATE_60;

  error = fc2SetVideoModeAndFrameRate (fc->context, video_mode, frame_rate);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2SetVideoModeAndFrameRate: %d", error);
    return;
  }

  error = fc2CreateImage (&fc->image);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2CreateImage: %d", error);
    return;
  }

  error = fc2CreateImage (&fc->image2);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2CreateImage: %d", error);
    return;
  }

  kr_fc2_set_timestamping (fc, TRUE);

  error = fc2StartCapture (fc->context);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2StartCapture: %d", error);
    return;
  }
}

void kr_fc2_capture_stop (kr_fc2_t *fc) {

  fc2Error error;

  error = fc2StopCapture (fc->context);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2StopCapture: %d", error );
  }

  error = fc2DestroyImage (&fc->image);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2DestroyImage: %d", error);
  }
  error = fc2DestroyImage (&fc->image2);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2DestroyImage: %d", error);
  }
}

void kr_fc2_getinfo (kr_fc2_t *fc) {

  fc2GetLibraryVersion (&fc->version);

  sprintf (
      fc->version_str,
      "FlyCapture2 library version: %d.%d.%d.%d\n",
      fc->version.major, fc->version.minor,
      fc->version.type, fc->version.build );

  printk (fc->version_str);
}

int kr_fc2_detect (kr_fc2_t *fc) {

  fc2Error error;

  error = fc2GetNumOfCameras (fc->context, &fc->num_cameras);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2GetNumOfCameras: %d\n", error);
    return -1;
  }

  if (fc->num_cameras == 0) {
    printk ("No flycapture cameras detected!");
    return 0;
  }

  error = fc2GetCameraFromIndex (fc->context, 0, &fc->guid );
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2GetCameraFromIndex: %d\n", error);
    return -1;
  }    

  error = fc2Connect (fc->context, &fc->guid);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2Connect: %d\n", error);
    return -1;
  }

  error = fc2GetCameraInfo (fc->context, &fc->cam_info);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2GetCameraInfo: %d\n", error);
    return -1;
  }

  printk (
      "*** CAMERA INFORMATION ***\n"
      "Serial number - %u\n"
      "Camera model - %s\n"
      "Camera vendor - %s\n"
      "Sensor - %s\n"
      "Resolution - %s\n"
      "Firmware version - %s\n"
      "Firmware build time - %s\n",
      fc->cam_info.serialNumber,
      fc->cam_info.modelName,
      fc->cam_info.vendorName,
      fc->cam_info.sensorInfo,
      fc->cam_info.sensorResolution,
      fc->cam_info.firmwareVersion,
      fc->cam_info.firmwareBuildTime );

  return fc->num_cameras;
}

int kr_fc2_destroy (kr_fc2_t **fc) {

  fc2Error error;

  if ((fc != NULL) && (*fc != NULL)) {
    error = fc2DestroyContext ((*fc)->context);
    if (error != FC2_ERROR_OK) {
      printke ("Error in fc2DestroyContext: %d\n", error);
    }
    free (*fc);
    *fc = NULL;
    return 0;
  }
  return -1;
}

kr_fc2_t *kr_fc2_create () {

  fc2Error error;
  kr_fc2_t *fc;

  fc = calloc (1, sizeof(kr_fc2_t));

  error = fc2CreateContext (&fc->context);
  if (error != FC2_ERROR_OK) {
    printke ("Error in fc2CreateContext: %d\n", error);
    free (fc);
    return NULL;
  }

  kr_fc2_getinfo (fc);
  kr_fc2_detect (fc);

  return fc;
}
