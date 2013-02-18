#include "krad_coreaudio.h"

OSStatus SetDefaultInputDeviceAsCurrent(coreaudio_t *coreaudio) {

    UInt32 size;
    OSStatus err =noErr;
    size = sizeof(AudioDeviceID);

    AudioDeviceID inputDevice;
    err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice,
                                                  &size, 
                                                  &inputDevice);

  

    err =AudioUnitSetProperty(coreaudio->au,
                         kAudioOutputUnitProperty_CurrentDevice, 
                         kAudioUnitScope_Global, 
                         0, 
                         &inputDevice, 
                         sizeof(inputDevice));

   return err;

}


int coreaudio_process (void *user_ptr,
                       AudioUnitRenderActionFlags *ioActionFlags,
                       const AudioTimeStamp *inTimeStamp,
                       UInt32 inBusNumber,
                       UInt32 inNumberFrames,
                       AudioBufferList *ioData) {

  int b;

  coreaudio_t *coreaudio = (coreaudio_t *)user_ptr;

  printf ("got a coreaudio process callback! %u\n", inNumberFrames);


  return noErr;
}

void coreaudio_start (coreaudio_t *coreaudio) {
  AudioOutputUnitStart (coreaudio->au);
}

void coreaudio_stop (coreaudio_t *coreaudio) {
  AudioOutputUnitStop (coreaudio->au);
}

void coreaudio_destroy (coreaudio_t *coreaudio) {
  AudioUnitUninitialize (coreaudio->au);
}

coreaudio_t *coreaudio_create () {

  //OSStatus res;

  //int i;
  
  coreaudio_t *coreaudio;
  
  coreaudio = calloc (1, sizeof (coreaudio_t));


 	AudioComponent comp;
    AudioComponentDescription desc;
    //AudioComponentInstance auHAL;

    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_HALOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;

    //Finds a component that meets the desc spec's
    comp = AudioComponentFindNext(NULL, &desc);
    if (comp == NULL) exit (-1);

     //gains access to the services provided by the component
    AudioComponentInstanceNew(comp, &coreaudio->au);


	AudioUnitInitialize (coreaudio->au);



 UInt32 enableIO;
     UInt32 size=0;

     //When using AudioUnitSetProperty the 4th parameter in the method
     //refer to an AudioUnitElement. When using an AudioOutputUnit
     //the input element will be '1' and the output element will be '0'.


      enableIO = 1;
      AudioUnitSetProperty(coreaudio->au,
                kAudioOutputUnitProperty_EnableIO,
                kAudioUnitScope_Input,
                1, // input element
                &enableIO,
                sizeof(enableIO));

      enableIO = 0;
      AudioUnitSetProperty(coreaudio->au,
                kAudioOutputUnitProperty_EnableIO,
                kAudioUnitScope_Output,
                0,   //output element
                &enableIO,
                sizeof(enableIO));


			SetDefaultInputDeviceAsCurrent(coreaudio);


		 AudioStreamBasicDescription DeviceFormat;
		    AudioStreamBasicDescription DesiredFormat;
		   //Use CAStreamBasicDescriptions instead of 'naked' 
		   //AudioStreamBasicDescriptions to minimize errors.
		   //CAStreamBasicDescription.h can be found in the CoreAudio SDK.

		    size = sizeof(AudioStreamBasicDescription);

		     //Get the input device format
		    AudioUnitGetProperty (coreaudio->au,
		                                   kAudioUnitProperty_StreamFormat,
		                                   kAudioUnitScope_Input,
		                                   1,
		                                   &DeviceFormat,
		                                   &size);

		    //set the desired format to the device's sample rate
		    DesiredFormat.mSampleRate =  DeviceFormat.mSampleRate;

		     //set format to output scope
		    AudioUnitSetProperty(
		                            coreaudio->au,
		                            kAudioUnitProperty_StreamFormat,
		                            kAudioUnitScope_Output,
		                            1,
		                            &DesiredFormat,
		                            sizeof(AudioStreamBasicDescription));

								AudioUnitInitialize (coreaudio->au);
								//AudioOutputUnitStart (coreaudio->au);


		   AURenderCallbackStruct input;
		 input.inputProc = coreaudio_process;
		  input.inputProcRefCon = (void*)coreaudio;

			AudioUnitSetProperty(
		            coreaudio->au, 
		            kAudioOutputUnitProperty_SetInputCallback, 
		            kAudioUnitScope_Global,
		            0,
		            &input, 
		            sizeof(input));



				AudioUnitInitialize (coreaudio->au);
				//AudioOutputUnitStart (coreaudio->au);




  return coreaudio;
}

void coreaudio_test () {
	
	coreaudio_t *coreaudio;

	coreaudio = coreaudio_create ();
	printf ("yay!\n");
	coreaudio_start (coreaudio);
	while (1) {
		usleep (100000);
	}

	return 0;
}


