#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AudioComponent.h>

typedef struct coreaudio_St coreaudio_t;

struct coreaudio_St {
  AudioUnit au;
};

void coreaudio_start (coreaudio_t *coreaudio);
void coreaudio_stop (coreaudio_t *coreaudio);
void coreaudio_destroy (coreaudio_t *coreaudio);
coreaudio_t *coreaudio_create ();

void coreaudio_test ();


