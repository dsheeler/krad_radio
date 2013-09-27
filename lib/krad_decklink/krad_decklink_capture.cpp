#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#include "DeckLinkAPI.h"
#include "krad_decklink_capture.h"

DeckLinkCaptureDelegate::DeckLinkCaptureDelegate() : refs(0) {}
DeckLinkCaptureDelegate::~DeckLinkCaptureDelegate() {}

ULONG DeckLinkCaptureDelegate::AddRef (void) {
  __sync_fetch_and_add ( &refs, 1 );
  return refs;
}

ULONG DeckLinkCaptureDelegate::Release (void) {
  __sync_fetch_and_sub( &refs, 1 );
  if (refs == 0) {
    delete this;
    return 0;
  }
  return refs;
}

HRESULT DeckLinkCaptureDelegate::VideoInputFrameArrived (IDeckLinkVideoInputFrame *video_frame,
                                                         IDeckLinkAudioInputPacket *audio_frame) {

  void *frame_data;
  long int frame_data_size;
  void *audio_data;
  IDeckLinkTimecode *timecode;
  BMDTimecodeFormat timecodeFormat;
  int audio_frames;

#ifdef KR_LINUX
  const char *timecodeString;
#endif
#ifdef FRAK_MACOSX
  CFStringRef timecodeString;
#endif

  timecodeString = NULL;
  timecodeFormat = 0;
  frame_data_size = 0;

  if ((audio_frame == NULL) || (video_frame == NULL) || (video_frame->GetFlags() & bmdFrameHasNoInputSource)) {
    if (krad_decklink_capture->skipped_frames == 0) {
      printke ("Decklink A/V Capture skipped a frame!");
    }
    krad_decklink_capture->skipped_frames++;
  }

  if (krad_decklink_capture->skipped_frames > 0) {
    printke ("Decklink A/V Capture got a frame after skipping %d", krad_decklink_capture->skipped_frames);
    krad_decklink_capture->skipped_frames = 0;
  }

  if (audio_frame) {
    audio_frame->GetBytes(&audio_data);
    audio_frames = audio_frame->GetSampleFrameCount();
    if (krad_decklink_capture->audio_frames_callback != NULL) {
      krad_decklink_capture->audio_frames_callback (krad_decklink_capture->callback_pointer, audio_data, audio_frames);
    }
    krad_decklink_capture->audio_frames += audio_frames;
  }

  if (krad_decklink_capture->skip_frame == 1) {
    krad_decklink_capture->skip_frame = 0;
    return S_OK;
  }

  if (krad_decklink_capture->skip_frame == 0) {
    if ((krad_decklink_capture->display_mode == bmdModeHD720p60) ||
      (krad_decklink_capture->display_mode == bmdModeHD720p5994) ||
      (krad_decklink_capture->display_mode == bmdModeHD1080p5994) ||
      (krad_decklink_capture->display_mode == bmdModeHD1080p6000)) {

      krad_decklink_capture->skip_frame = 1;
    }
  }

  if (video_frame) {
    if (timecodeFormat != 0) {
      if (video_frame->GetTimecode(timecodeFormat, &timecode) == S_OK) {
        timecode->GetString(&timecodeString);
      }
    }
    frame_data_size = video_frame->GetRowBytes() * video_frame->GetHeight();
    if (timecodeString) {
#ifdef KR_LINUX
      free ((void*)timecodeString);
#endif
#ifdef FRAK_MACOSX
      CFRelease(timecodeString);
#endif
    }
    video_frame->GetBytes(&frame_data);
    if (krad_decklink_capture->video_frame_callback != NULL) {
      krad_decklink_capture->video_frame_callback (krad_decklink_capture->callback_pointer, frame_data, frame_data_size);
    }
    krad_decklink_capture->video_frames++;
  }
  return S_OK;
}

HRESULT DeckLinkCaptureDelegate::VideoInputFormatChanged (BMDVideoInputFormatChangedEvents events,
                                                          IDeckLinkDisplayMode *mode,
                                                          BMDDetectedVideoInputFormatFlags) {
  printke ("ruh oh! video format changed?!?\n");
  return S_OK;
}

extern "C" {

krad_decklink_capture_t *krad_decklink_capture_create (int device) {

  int d;
  krad_decklink_capture_t *krad_decklink_capture = (krad_decklink_capture_t *)calloc(1, sizeof(krad_decklink_capture_t));

  krad_decklink_capture->device = device;
  krad_decklink_capture->inputFlags = 0;
  krad_decklink_capture_set_audio_input (krad_decklink_capture, "");
  krad_decklink_capture_set_video_input (krad_decklink_capture, "");
  krad_decklink_capture->audio_sample_rate = bmdAudioSampleRate48kHz;
  krad_decklink_capture->audio_channels = 2;
  krad_decklink_capture->audio_bit_depth = 16;

  krad_decklink_capture->deckLinkIterator = CreateDeckLinkIteratorInstance();

  if (!krad_decklink_capture->deckLinkIterator) {
    printke("Krad Decklink: No DeckLink drivers installed.");
    free (krad_decklink_capture);
    return NULL;
  }

  for (d = 0; d < 64; d++) {

    krad_decklink_capture->result = krad_decklink_capture->deckLinkIterator->Next(&krad_decklink_capture->deckLink);
    if (krad_decklink_capture->result != S_OK) {
      printke ("Krad Decklink: No DeckLink devices found.");
      free (krad_decklink_capture);
      return NULL;
    }

    if (d == krad_decklink_capture->device) {
      break;
    }

  }
  return krad_decklink_capture;
}

void krad_decklink_capture_set_audio_input (krad_decklink_capture_t *krad_decklink_capture, char *audio_input) {

  krad_decklink_capture->audio_input = bmdAudioConnectionEmbedded;

  if (strlen(audio_input)) {
    if ((strstr(audio_input, "Analog") != NULL) || (strstr(audio_input, "analog") != NULL)) {
      printk ("Krad Decklink: Selected Analog Audio Input");
      krad_decklink_capture->audio_input = bmdAudioConnectionAnalog;
      return;
    }
    if ((strstr(audio_input, "AESEBU") != NULL) || (strstr(audio_input, "aesebu") != NULL) ||
      (strstr(audio_input, "SPDIF") != NULL) || (strstr(audio_input, "spdif") != NULL)) {
      printk ("Krad Decklink: Selected AESEBU Audio Input");
      krad_decklink_capture->audio_input = bmdAudioConnectionAESEBU;
      return;
    }
  }
  printk ("Krad Decklink: Selected Embedded Audio Input");
}

void krad_decklink_capture_set_video_input (krad_decklink_capture_t *krad_decklink_capture, char *video_input) {

  krad_decklink_capture->video_input = bmdVideoConnectionSDI;

  if (strlen(video_input)) {

    if ((strstr(video_input, "HDMI") != NULL) || (strstr(video_input, "hdmi") != NULL)) {
      printk ("Krad Decklink: Selected HDMI Video Input");
      krad_decklink_capture->video_input = bmdVideoConnectionHDMI;
      return;
    }

    if ((strstr(video_input, "OpticalSDI") != NULL) || (strstr(video_input, "opticalsdi") != NULL)) {
      printk ("Krad Decklink: Selected OpticalSDI Video Input");
      krad_decklink_capture->video_input = bmdVideoConnectionOpticalSDI;
      return;
    }

    if ((strstr(video_input, "Component") != NULL) || (strstr(video_input, "component") != NULL)) {
      printk ("Krad Decklink: Selected Component Video Input");
      krad_decklink_capture->video_input = bmdVideoConnectionComponent;
      return;
    }

    if ((strstr(video_input, "Composite") != NULL) || (strstr(video_input, "composite") != NULL)) {
      printk ("Krad Decklink: Selected Composite Video Input");
      krad_decklink_capture->video_input = bmdVideoConnectionComposite;
      return;
    }

    if ((strstr(video_input, "svideo") != NULL) || (strstr(video_input, "Svideo") != NULL)) {
      printk ("Krad Decklink: Selected svideo Video Input");
      krad_decklink_capture->video_input = bmdVideoConnectionSVideo;
      return;
    }
  }

  printk ("Krad Decklink: Selected SDI Video Input");
}

void krad_decklink_capture_set_video_mode (krad_decklink_capture_t *krad_decklink_capture,
                                           int width, int height,
                                           int fps_numerator, int fps_denominator) {


  krad_decklink_capture->width = width;
  krad_decklink_capture->height = height;
  krad_decklink_capture->fps_numerator = fps_numerator;
  krad_decklink_capture->fps_denominator = fps_denominator;

  krad_decklink_capture->pixel_format = bmdFormat8BitYUV;
  //krad_decklink_capture->pixel_format = bmdFormat8BitARGB;

  if ((krad_decklink_capture->width == 720) && (krad_decklink_capture->height == 486)) {
    if ((krad_decklink_capture->fps_numerator == 30000) && (krad_decklink_capture->fps_denominator == 1001)) {
      krad_decklink_capture->display_mode = bmdModeNTSC;
    }
    if ((krad_decklink_capture->fps_numerator == 60000) && (krad_decklink_capture->fps_denominator == 1001)) {
      krad_decklink_capture->display_mode = bmdModeNTSCp;
    }
  }

  if ((krad_decklink_capture->width == 720) && (krad_decklink_capture->height == 576)) {
    if (((krad_decklink_capture->fps_numerator == 25) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 25000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdModePAL;
    }
    if (((krad_decklink_capture->fps_numerator == 50) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 50000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdModePALp;
    }
  }

  if ((krad_decklink_capture->width == 1280) && (krad_decklink_capture->height == 720)) {
    if (((krad_decklink_capture->fps_numerator == 50) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 50000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdModeHD720p50;
    }
    if ((krad_decklink_capture->fps_numerator == 60000) && (krad_decklink_capture->fps_denominator == 1001)) {
      krad_decklink_capture->display_mode = bmdModeHD720p5994;
    }
    if (((krad_decklink_capture->fps_numerator == 60) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 60000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdModeHD720p60;
    }
  }

  if ((krad_decklink_capture->width == 1920) && (krad_decklink_capture->height == 1080)) {
    if ((krad_decklink_capture->fps_numerator == 24000) && (krad_decklink_capture->fps_denominator == 1001)) {
      krad_decklink_capture->display_mode = bmdModeHD1080p2398;
    }
    if (((krad_decklink_capture->fps_numerator == 24) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 24000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdModeHD1080p24;
    }
    if (((krad_decklink_capture->fps_numerator == 25) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 25000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdModeHD1080p25;
    }
    if ((krad_decklink_capture->fps_numerator == 30000) && (krad_decklink_capture->fps_denominator == 1001)) {
      krad_decklink_capture->display_mode = bmdModeHD1080p2997;
    }
    if (((krad_decklink_capture->fps_numerator == 30) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 30000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdModeHD1080p30;
    }
    if (((krad_decklink_capture->fps_numerator == 50) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 50000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdModeHD1080p50;
    }
    if ((krad_decklink_capture->fps_numerator == 60000) && (krad_decklink_capture->fps_denominator == 1001)) {
      krad_decklink_capture->display_mode = bmdModeHD1080p5994;
    }
    if (((krad_decklink_capture->fps_numerator == 60) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 60000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdModeHD1080p6000;
    }
  }

  if ((krad_decklink_capture->width == 2048) && (krad_decklink_capture->height == 1556)) {
    if ((krad_decklink_capture->fps_numerator == 24000) && (krad_decklink_capture->fps_denominator == 1001)) {
      krad_decklink_capture->display_mode = bmdMode2k2398;
    }
    if (((krad_decklink_capture->fps_numerator == 24) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 24000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdMode2k24;
    }
    if (((krad_decklink_capture->fps_numerator == 25) && (krad_decklink_capture->fps_denominator == 1)) ||
       ((krad_decklink_capture->fps_numerator == 25000) && (krad_decklink_capture->fps_denominator == 1000))) {
      krad_decklink_capture->display_mode = bmdMode2k25;
    }
  }
}

void krad_decklink_capture_set_video_callback (krad_decklink_capture_t *krad_decklink_capture, int video_frame_callback(void *, void *, int)) {
  krad_decklink_capture->video_frame_callback = video_frame_callback;
}

void krad_decklink_capture_set_audio_callback (krad_decklink_capture_t *krad_decklink_capture, int audio_frames_callback(void *, void *, int)) {
  krad_decklink_capture->audio_frames_callback = audio_frames_callback;
}

void krad_decklink_capture_set_callback_pointer (krad_decklink_capture_t *krad_decklink_capture, void *callback_pointer) {
  krad_decklink_capture->callback_pointer = callback_pointer;
}

void krad_decklink_capture_set_verbose (krad_decklink_capture_t *krad_decklink_capture, int verbose) {
  krad_decklink_capture->verbose = verbose;
}

void krad_decklink_capture_start (krad_decklink_capture_t *krad_decklink_capture) {

  krad_decklink_capture->result = krad_decklink_capture->deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&krad_decklink_capture->deckLinkInput);
  if (krad_decklink_capture->result != S_OK) {
    printke ("Krad Decklink: Fail QueryInterface\n");
  }

  krad_decklink_capture->result = krad_decklink_capture->deckLink->QueryInterface(IID_IDeckLinkConfiguration, (void**)&krad_decklink_capture->deckLinkConfiguration);
  if (krad_decklink_capture->result != S_OK) {
    printke ("Krad Decklink: Fail QueryInterface to get configuration\n");
  } else {

    krad_decklink_capture->result = krad_decklink_capture->deckLinkConfiguration->SetInt(bmdDeckLinkConfigAudioInputConnection, krad_decklink_capture->audio_input);
    if (krad_decklink_capture->result != S_OK) {
      printke ("Krad Decklink: Failed to set audio input connection");
    }

    krad_decklink_capture->result = krad_decklink_capture->deckLinkConfiguration->SetInt(bmdDeckLinkConfigVideoInputConnection, krad_decklink_capture->video_input);
    if (krad_decklink_capture->result != S_OK) {
      printke ("Krad Decklink: Failed to set video input connection");
    }
  }

  krad_decklink_capture->delegate = new DeckLinkCaptureDelegate();
  krad_decklink_capture->delegate->krad_decklink_capture = krad_decklink_capture;
  krad_decklink_capture->deckLinkInput->SetCallback(krad_decklink_capture->delegate);

  krad_decklink_capture->result = krad_decklink_capture->deckLinkInput->EnableVideoInput(krad_decklink_capture->display_mode, krad_decklink_capture->pixel_format, krad_decklink_capture->inputFlags);
  if (krad_decklink_capture->result != S_OK) {
    printke ("Krad Decklink: Fail EnableVideoInput\n");
  }

  krad_decklink_capture->result = krad_decklink_capture->deckLinkInput->EnableAudioInput(krad_decklink_capture->audio_sample_rate, krad_decklink_capture->audio_bit_depth, krad_decklink_capture->audio_channels);
  if (krad_decklink_capture->result != S_OK) {
    printke ("Krad Decklink: Fail EnableAudioInput\n");
  }

  krad_decklink_capture->result = krad_decklink_capture->deckLinkInput->StartStreams();
  if (krad_decklink_capture->result != S_OK) {
    printke ("Krad Decklink: Fail StartStreams\n");
  }
}

void krad_decklink_capture_stop (krad_decklink_capture_t *krad_decklink_capture) {

    if (krad_decklink_capture->deckLinkInput != NULL) {

    krad_decklink_capture->result = krad_decklink_capture->deckLinkInput->StopStreams ();
    if (krad_decklink_capture->result != S_OK) {
      printke ("Krad Decklink: Fail StopStreams\n");
    }
    krad_decklink_capture->result = krad_decklink_capture->deckLinkInput->DisableVideoInput ();
    if (krad_decklink_capture->result != S_OK) {
      printke ("Krad Decklink: Fail DisableVideoInput\n");
    }
    krad_decklink_capture->result = krad_decklink_capture->deckLinkInput->DisableAudioInput ();
    if (krad_decklink_capture->result != S_OK) {
      printke ("Krad Decklink: Fail DisableAudioInput\n");
    }

    krad_decklink_capture->result = krad_decklink_capture->deckLinkConfiguration->Release();
    if (krad_decklink_capture->result != S_OK) {
      printke ("Krad Decklink: Fail to Release deckLinkConfiguration\n");
    }
    krad_decklink_capture->deckLinkConfiguration = NULL;

    krad_decklink_capture->result = krad_decklink_capture->deckLinkInput->Release();
    if (krad_decklink_capture->result != S_OK) {
      printke ("Krad Decklink: Fail Release\n");
    }
        krad_decklink_capture->deckLinkInput = NULL;
    }

    if (krad_decklink_capture->deckLink != NULL) {
        krad_decklink_capture->deckLink->Release();
        krad_decklink_capture->deckLink = NULL;
    }

  if (krad_decklink_capture->deckLinkIterator != NULL) {
    krad_decklink_capture->deckLinkIterator->Release();
  }

  free (krad_decklink_capture);
}

#ifdef KR_LINUX
void krad_decklink_capture_info () {

  IDeckLink *deckLink;
  IDeckLinkInput *deckLinkInput;
  IDeckLinkIterator *deckLinkIterator;
  IDeckLinkDisplayModeIterator *displayModeIterator;
  IDeckLinkDisplayMode *displayMode;

  HRESULT result;
  int displayModeCount;
  char *displayModeString;

  displayModeString = NULL;
  displayModeCount = 0;

  deckLinkIterator = CreateDeckLinkIteratorInstance();

  if (!deckLinkIterator) {
    printke ("Krad Decklink: This application requires the DeckLink drivers installed.\n");
  }

  /* Connect to the first DeckLink instance */
  result = deckLinkIterator->Next(&deckLink);
  if (result != S_OK) {
    printke ("Krad Decklink: No DeckLink PCI cards found.\n");
  }

  result = deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput);
  if (result != S_OK) {
    printke ("Krad Decklink: Fail QueryInterface\n");
  }

  result = deckLinkInput->GetDisplayModeIterator(&displayModeIterator);
  if (result != S_OK) {
    printke ("Krad Decklink: Could not obtain the video output display mode iterator - result = %08x\n", result);
  }

  while (displayModeIterator->Next(&displayMode) == S_OK) {

    result = displayMode->GetName((const char **) &displayModeString);

    if (result == S_OK) {

      BMDTimeValue frameRateDuration, frameRateScale;
      displayMode->GetFrameRate(&frameRateDuration, &frameRateScale);

      printk ("%2d:  %-20s \t %li x %li \t %g FPS\n",
        displayModeCount, displayModeString, displayMode->GetWidth(), displayMode->GetHeight(),
        (double)frameRateScale / (double)frameRateDuration);

            free (displayModeString);
      displayModeCount++;
    }

    displayMode->Release();
  }

  if (displayModeIterator != NULL) {
    displayModeIterator->Release();
    displayModeIterator = NULL;
  }

    if (deckLinkInput != NULL) {
        deckLinkInput->Release();
        deckLinkInput = NULL;
    }

    if (deckLink != NULL) {
        deckLink->Release();
        deckLink = NULL;
    }

  if (deckLinkIterator != NULL) {
    deckLinkIterator->Release();
  }
}
#endif

int krad_decklink_cpp_detect_devices () {

  IDeckLinkIterator *deckLinkIterator;
  IDeckLink *deckLink;
  int device_count;
  //HRESULT result;

  device_count = 0;

  deckLinkIterator = CreateDeckLinkIteratorInstance();

  if (deckLinkIterator == NULL) {
    printke ("krad_decklink_detect_devices: The DeckLink drivers may not be installed.");
    return 0;
  }

  while (deckLinkIterator->Next(&deckLink) == S_OK) {
    device_count++;
    deckLink->Release();
  }

  deckLinkIterator->Release();

  return device_count;
}

int krad_decklink_cpp_get_device_name (int device_num, char *device_name) {

  IDeckLinkIterator *deckLinkIterator;
  IDeckLink *deckLink;
  int device_count;
  HRESULT result;
#ifdef KR_LINUX
  char *device_name_temp;
#endif
#ifdef FRAK_MACOSX
  CFStringRef device_name_temp;
#endif
  int ret;

  device_name_temp = NULL;
  device_count = 0;
  ret = 0;

  deckLinkIterator = CreateDeckLinkIteratorInstance();

  if (deckLinkIterator == NULL) {
    printke ("krad_decklink_detect_devices: The DeckLink drivers may not be installed.");
    return ret;
  }

  while (deckLinkIterator->Next(&deckLink) == S_OK) {

    if (device_count == device_num) {
#ifdef KR_LINUX
      result = deckLink->GetModelName((const char **) &device_name_temp);
      if (result == S_OK) {
        strcpy (device_name, device_name_temp);
        free (device_name_temp);
        ret = 1;
#endif
#ifdef FRAK_MACOSX
      result = deckLink->GetModelName(&device_name_temp);
      if (result == S_OK) {
        CFStringGetCString(device_name_temp, device_name, 64, kCFStringEncodingMacRoman);
        CFRelease(device_name_temp);
        ret = 1;
#endif

       } else {
         strcpy(device_name, "Unknown Error in GetModelName");
       }
        deckLink->Release();
        deckLinkIterator->Release();
        return ret;
       }

      device_count++;
      deckLink->Release();
    }

    deckLinkIterator->Release();
    sprintf(device_name, "Could not get a device name for device %d", device_num);
    return ret;
  }
}
