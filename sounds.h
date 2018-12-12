#pragma once
#include "shapes.h"
#include <portaudio.h>
#if defined(__APPLE__)
  #include <pa_mac_core.h>
#elif defined(__linux__)
  #include <pa_linux_alsa.h>
// #else
// #include <pa_win_ds.h>
// #include <pa_win_waveformat.h>
// #include <pa_win_wasapi.h>
// #include <pa_win_wdmks.h>
// #include <pa_win_wmme.h>
#endif

#define STANDARD_CHANNEL_COUNT 2
#define MAX_TRACKS_COUNT 6


typedef enum t_soundstate {

    SOUND_PLAYING,
    SOUND_PAUSED,
    SOUND_STOPPED,
    SOUND_INVALID

} soundState;

typedef enum t_soundtype {

    SOUNDTYPE_SOUNDTRACK = 0,
    SOUNDTYPE_AMBIENT = 1,
    SOUNDTYPE_SFX01 = 2,
    SOUNDTYPE_SFX02 = 3

} soundType;


typedef struct {
    unsigned long long frameIndex;
    unsigned long long maxFrameIndex;
    float *buffer;
    unsigned int channels;
    unsigned int sampleRate;
    unsigned int framesPerBuffer;

} soundData;

typedef struct t_sound {

    soundData soundData;
    soundState state;
    soundType type;
    float leftVolume;  // 0.f to 1.f
    float rightVolume; // 0.f to 1.f
    float masterVolume;
    bool onLoop;

} soundEffect;

typedef struct t_soundobj {

    v3 position;
    v3 upVector;
    v3 directionVector;
    float soundRadius; // Value when the volume reaches zero
    int sound_mask;
    int soundEffectID;

} soundObject;

typedef struct t_hearing {

    v3 position;
    v3 upVector;
    v3 directionVector;
    int sound_mask; 

} soundReceiver;

typedef struct t_stream {
    
    PaStream *stream;
    int *soundsID;
    int soundsCount;
    
} streamObject;



PaStream *stream;

//Spacial Sound
void updateSpacialSound(soundReceiver *receiver, soundObject *sources, int sourcesCount);

//Load
int loadFLACAudio(const char* filename, soundObject *object, soundType type, bool loopable);

//Free
void terminateSounds(void);

//Audio controls
void killAudio(soundObject *object);
void playAudio(soundObject *object);
void resumeAudio(soundObject *object);
void pauseAudio(soundObject *object);
void stopAudio(soundObject *object);
void shiftAudioState(soundObject *object);
