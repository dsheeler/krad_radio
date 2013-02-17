#include <CoreServices/CoreServices.h>
#include <AudioUnit/AudioUnit.h>


-framework', 'CoreAudio'
-framework', 'AudioUnit'
-framework', 'CoreServices'


AudioUnit au;


//reset
  res = AudioUnitReset (data->au, kAudioUnitScope_Global, 0);


// start / stop
  AudioOutputUnitStart (data->au);
  AudioOutputUnitStop (data->au);


//callback

process (void *user_ptr,
        AudioUnitRenderActionFlags *ioActionFlags,
        const AudioTimeStamp *inTimeStamp,
        UInt32 inBusNumber,
        UInt32 inNumberFrames,
        AudioBufferList *ioData)
{

	gint b;

	xmms_output_t *output = (xmms_output_t *)inRefCon;
	g_return_val_if_fail (output, kAudioHardwareUnspecifiedError);



	for (b = 0; b < ioData->mNumberBuffers; ++ b) {
		gint size;
		gint ret;

		size = ioData->mBuffers[b].mDataByteSize;

		ret = xmms_output_read (output, (gchar *)ioData->mBuffers[b].mData, size);
		if (ret == -1)
			ret = 0;

		if (ret < size) {
			memset (ioData->mBuffers[b].mData+ret, 0, size - ret);
		}
	}

	return noErr;
}



// Set format 

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



// Destroy

	AudioUnitUninitialize (data->au);
	CloseComponent (data->au);


// Create


	OSStatus res;
	ComponentDescription desc;
	AURenderCallbackStruct input;
	Component comp;
	AudioDeviceID device = 0;
	UInt32 size = sizeof (device);
	gint i;

	g_return_val_if_fail (output, FALSE);

	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_DefaultOutput;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;

	comp = FindNextComponent (NULL, &desc);
	if (!comp) {
		xmms_log_error ("Couldn't find that component in my list!");
		return FALSE;
	}

	data = g_new0 (xmms_ca_data_t, 1);

	res = OpenAComponent (comp, &data->au);
	if (comp == NULL) {
		xmms_log_error ("Opening component failed!");
		g_free (data);
		return FALSE;
	}

	input.inputProc = PROCESS_CALLBACK_HERE;
	input.inputProcRefCon = (void*)output; ??? user pointer?

	res = AudioUnitSetProperty (data->au, kAudioUnitProperty_SetRenderCallback,
	                            kAudioUnitScope_Input, 0,
	                            &input, sizeof (input));

	if (res) {
		xmms_log_error ("Set Callback failed!");
		g_free (data);
		return FALSE;
	}

	res = AudioUnitInitialize (data->au);
	if (res) {
		xmms_log_error ("Audio Unit wouldn't initialize!");
		g_free (data);
		return FALSE;
	}

	XMMS_DBG ("CoreAudio initialized!");

	res = AudioUnitGetProperty (data->au,
	                            kAudioOutputUnitProperty_CurrentDevice,
	                            kAudioUnitScope_Global,
	                            0,
	                            &device,
	                            &size);

	if (res) {
		xmms_log_error ("getprop failed!");
		g_free (data);
		return FALSE;
	}

