#include <CoreServices/CoreServices.h>
#include <AudioUnit/AudioUnit.h>

-framework', 'CoreAudio'
-framework', 'AudioUnit'
-framework', 'CoreServices'

typedef struct coreaudio_St coreaudio_t;

struct coreaudio_St {
  AudioUnit au;
};

void coreaudio_run (coreaudio_t *coreaudio);
void coreaudio_destroy (coreaudio_t *coreaudio);
coreaudio_t *coreaudio_create ();



