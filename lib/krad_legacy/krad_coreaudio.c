#include "krad_coreaudio.h"

/*
void monkey () {
//reset
  res = AudioUnitReset (data->au, kAudioUnitScope_Global, 0);


// start / stop
  AudioOutputUnitStart (data->au);
  AudioOutputUnitStop (data->au);

//

  AudioStreamBasicDescription fmt;

  guint flags;

  data = xmms_output_private_data_get (output);

  fmt.mFormatID = kAudioFormatLinearPCM;
  fmt.mFramesPerPacket = 1;

  fmt.mSampleRate = xmms_stream_type_get_int (stype, XMMS_STREAM_TYPE_FMT_SAMPLERATE);
  fmt.mChannelsPerFrame = xmms_stream_type_get_int (stype, XMMS_STREAM_TYPE_FMT_CHANNELS);

  flags = kAudioFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
  fmt.mFormatFlags = flags;


  fmt.mBytesPerFrame = fmt.mBitsPerChannel * fmt.mChannelsPerFrame / 8;
  fmt.mBytesPerPacket = fmt.mBytesPerFrame * fmt.mFramesPerPacket;

  res = AudioUnitSetProperty (data->au, kAudioUnitProperty_StreamFormat,
                              kAudioUnitScope_Input, 0, &fmt,
                              sizeof (AudioStreamBasicDescription));


}
*/


int coreaudio_process (void *user_ptr,
                       AudioUnitRenderActionFlags *ioActionFlags,
                       const AudioTimeStamp *inTimeStamp,
                       UInt32 inBusNumber,
                       UInt32 inNumberFrames,
                       AudioBufferList *ioData) {

  int b;

  coreaudio_t *coreaudio = (coreaudio_t *)user_ptr;

  printf ("got a coreaudio process callback!\n");

  for (b = 0; b < ioData->mNumberBuffers; ++ b) {
    gint size;
    gint ret;

    size = ioData->mBuffers[b].mDataByteSize;

    //ret = xmms_output_read (output, (gchar *)ioData->mBuffers[b].mData, size);
    if (ret == -1)
      ret = 0;

    if (ret < size) {
      memset (ioData->mBuffers[b].mData+ret, 0, size - ret);
    }
  }

  return noErr;
}


void coreaudio_destroy (coreaudio_t *coreaudio) {
  AudioUnitUninitialize (coreaudio->au);
  CloseComponent (coreaudio->au);
}


coreaudio_t *coreaudio_create () {

  OSStatus res;
  ComponentDescription desc;
  AURenderCallbackStruct input;
  Component comp;
  AudioDeviceID device = 0;
  UInt32 size = sizeof (device);
  gint i;
  
  coreaudio_t *coreaudio;
  
  coreaudio = calloc (1, sizeof (coreaudio_t));

  desc.componentType = kAudioUnitType_Output;
  desc.componentSubType = kAudioUnitSubType_DefaultOutput;
  desc.componentManufacturer = kAudioUnitManufacturer_Apple;
  desc.componentFlags = 0;
  desc.componentFlagsMask = 0;

  comp = FindNextComponent (NULL, &desc);
  if (!comp) {
    printf ("Couldn't find that component in my list!\n");
    free (coreaudio);
    return NULL;
  }

  res = OpenAComponent (comp, &coreaudio->au);
  if (comp == NULL) {
    printf ("Opening component failed!\n");
    return NULL;
  }

  input.inputProc = coreaudio_process;
  input.inputProcRefCon = (void*)coreaudio;

  res = AudioUnitSetProperty (coreaudio->au, kAudioUnitProperty_SetRenderCallback,
                              kAudioUnitScope_Input, 0,
                              &input, sizeof (input));

  if (res) {
    printf ("Set Callback failed!\n");
    free (coreaudio);
    return NULL;
  }

  res = AudioUnitInitialize (data->au);
  if (res) {
    printf ("Audio Unit wouldn't initialize!\n");
    free (coreaudio);
    return NULL;
  }

  printf ("CoreAudio initialized!\n");

  res = AudioUnitGetProperty (data->au,
                              kAudioOutputUnitProperty_CurrentDevice,
                              kAudioUnitScope_Global,
                              0,
                              &device,
                              &size);

  if (res) {
    printf ("getprop failed!\n");
    free (coreaudio);
    return NULL;
  }

  return 0;
}
