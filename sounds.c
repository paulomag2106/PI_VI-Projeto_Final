#include "sounds.h"
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

#define SOUNDEFFECT_MAX 30




static soundEffect effects[SOUNDEFFECT_MAX];
int activeEffects[MAX_TRACKS_COUNT] = {-1,-1,-1,-1,-1,-1};

void printActive() {
    
    printf("%d %d %d %d %d %d\n", activeEffects[0], activeEffects[1], activeEffects[2], activeEffects[3], activeEffects[4], activeEffects[5]);
    
}

void deactivateSoundEffect(int effectID) {
    
    bool found = false;
    int index = 0;
    
    for(int i = 0; i < MAX_TRACKS_COUNT; i++) {
        
        if(activeEffects[i] == effectID) {
            activeEffects[i] = -1;
            found = true;
            index = i;
            break;
        }
    }
    
    for(int i = index + 1; i < MAX_TRACKS_COUNT; i++) {
        
        activeEffects[i-1] = activeEffects[i];
    }
    
    activeEffects[MAX_TRACKS_COUNT - 1] = -1;
    
    printf("Deactivated %d.\n",effectID);
    printActive();
    
    if(!found) printf("ERROR: Tried to deactivate SoundEffect with ID %d that was not active.\n", effectID);
    
}


void activateSoundEffect(int effectID) {
    
    bool hasSpace = false;
    
    for(int i = 0; i < MAX_TRACKS_COUNT; i++) {
        
        if(activeEffects[i] == effectID) {
            printf("WARNING: SoundEffect with ID %d is already activated.\n", effectID);
            return;
        }
        
        else if(activeEffects[i] < 0) {
            activeEffects[i] = effectID;
            hasSpace = true;
            break;
        }
    }
    
    printf("Activated %d.\n",effectID);
    printActive();
    
    if(!hasSpace) printf("ERROR: Cant activate SoundEffect with ID %d. No empty slots available.\n", effectID);
}

bool errorHandler(PaError err) {
    if(err != paNoError) {
        printf("ERROR on sounds with code: %d\n", err);
        Pa_Terminate();
        return true;
    }
    
    return false;
}



static int callback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData ) {
    
    float *output = (float*)outputBuffer;
    
    for(int i = 0; i < framesPerBuffer; i++) {
        
        long outputIndex = (i * STANDARD_CHANNEL_COUNT);
        
        double left = 0x0, right = 0x0;
        int count = 0, sound, num = 0;
        
        while(num < MAX_TRACKS_COUNT) {
            
            sound = activeEffects[num];
            
            if(sound < 0) {
                break;
            }
            
            soundEffect *effect = &effects[sound];
            
            float *buffer = effect->soundData.buffer;
            int source_channels = effect->soundData.channels;
            long frameIndex = effect->soundData.frameIndex * source_channels;

            if(frameIndex + source_channels >= effect->soundData.maxFrameIndex) {
                
                if(effect->onLoop) {
                    effect->soundData.frameIndex = 0;
                }
                
                else {
                    effect->state = SOUND_STOPPED;
                    deactivateSoundEffect(sound);
                }
            }
            
            else if(effect->state == SOUND_PLAYING) {
                
                float v = effect->masterVolume, lv = effect->leftVolume, rv = effect->rightVolume;
                
                left += (buffer[frameIndex] * v * lv);
                
                if(source_channels > 1)  right += (buffer[frameIndex + 1] * v * rv);
                else                     right += (buffer[frameIndex] * v * rv);
                
                effect->soundData.frameIndex++;
                count++;
            }
            
            num++;
        }
        
        output[outputIndex] = (float) left ;// (float)count;
        output[outputIndex + 1] = (float) right ;// (float)count;
        
        //left = 0x0; right = 0x0; count = 0; sound = 0;
    }

    return paContinue;

}

void startAudioStreamOutput() {
    
    errorHandler(Pa_StartStream( stream ));
}

void stopAudioStreamOutput() {
    
    errorHandler(Pa_StopStream( stream ));
}

soundEffect *getSoundEffect(int effectID) {
    
    return &effects[effectID];
}

void killAudio(soundObject *object) {
    
    soundEffect *effect = getSoundEffect(object->soundEffectID);
    
    if(effect->state == SOUND_PLAYING || effect->state == SOUND_PAUSED)
        deactivateSoundEffect(object->soundEffectID);
    
    free(effect->soundData.buffer);
    effect->state = SOUND_INVALID;
    object->soundEffectID = -1;
}


void stopAudio(soundObject *object) {
    
    soundEffect *effect = getSoundEffect(object->soundEffectID);
    
    deactivateSoundEffect(object->soundEffectID);
    effect->state = SOUND_STOPPED;
    effect->soundData.frameIndex = 0;
}

void pauseAudio(soundObject *object) {
    
    printf("Pausing %d\n", object->soundEffectID);
    
    soundEffect *effect = getSoundEffect(object->soundEffectID);
    effect->state = SOUND_PAUSED;
}

void resumeAudio(soundObject *object) {
    
    printf("Resuming %d\n", object->soundEffectID);
    
    soundEffect *effect = getSoundEffect(object->soundEffectID);
    
    if(effect->state == SOUND_INVALID || effect->state == SOUND_STOPPED)
        activateSoundEffect(object->soundEffectID);
    
    effect->state = SOUND_PLAYING;
}

void shiftAudioState(soundObject *object) {
    
    printf("Shifting %d\n", object->soundEffectID);
    
    if( getSoundEffect(object->soundEffectID)->state == SOUND_PLAYING) pauseAudio(object);
    else if( getSoundEffect(object->soundEffectID)->state == SOUND_PAUSED) resumeAudio(object);
    else playAudio(object);
    
}

void playAudio(soundObject *object) {
    
    printf("Playing %d\n", object->soundEffectID);
    
    if(Pa_IsStreamActive( stream ) == 0) startAudioStreamOutput();
    
    soundEffect *effect = getSoundEffect(object->soundEffectID);
    
    if(effect->state != SOUND_PLAYING) {
        effect->soundData.frameIndex = 0;
        resumeAudio(object);
    }
}

void updateSpacialSound(soundReceiver *receiver, soundObject *sources, int sourcesCount) {
    
    for(int i = 0; i < sourcesCount; i++) {
        
        soundEffect *effect = getSoundEffect(sources[i].soundEffectID);
        
        v2 vector = newV2(sources[i].position.x - receiver->position.x,
                          sources[i].position.y - receiver->position.y);
        
        v2 direction = newV2(receiver->directionVector.x, receiver->directionVector.y);
        
        effect->masterVolume = (sources[i].soundRadius - distV2(vector)) / (sources[i].soundRadius);
        
        float num = ((vector.x * direction.x) + (vector.y * direction.y)) / (distV2(vector) * distV2(direction));
        
        float angle = acosf(num) * (180.f / M_PI);
        float way = (direction.x * vector.y) - (direction.y * vector.x);
        
        if(angle > 90.f) {
            angle = 90.f - (angle - 90.f);
        }
        
        //Vector is to the left of the direction
        if(way > 0.f) {
            
            effect->leftVolume = 1.f;
            effect->rightVolume = 1.f - ((angle / 90.f) * 0.9f);
        }
        
        //Vector is to the left of the direction
        else {
            
            effect->rightVolume = 1.f;
            effect->leftVolume = 1.f - ((angle / 90.f) * 0.9f);
        }
        
    }
}

int getFreeSoundEffectIndex() {
    
    for(int x = 0; x < SOUNDEFFECT_MAX; x++) {
        if(!getSoundEffect(x)->soundData.buffer) return x;
    }
    
    return -1;
    
}


int loadFLACAudio(const char* filename, soundObject *object, soundType type, bool loopable) {
    
    if(!object) {
        printf("Sound object is NULL.\n");
        return -1;
    }
    
    unsigned int channels;
    unsigned int sampleRate;
    drflac_uint64 totalSampleCount;
    float* pSampleData = drflac_open_and_decode_file_f32(filename, &channels, &sampleRate, &totalSampleCount);
    
    if(!pSampleData) {
        printf("Failed to open file %s.\n", filename);
        return -1;
    }
    
    soundData sound_data;
    sound_data.channels = channels;
    sound_data.frameIndex = 0;
    sound_data.sampleRate = sampleRate;
    sound_data.maxFrameIndex = totalSampleCount;
    sound_data.buffer = pSampleData;
    
    int effectID = getFreeSoundEffectIndex();
    
    if(effectID < 0) {
        printf("No available slots to open SoundEffect.\n");
        free(pSampleData);
        return -1;
    }
    
    soundEffect *data = getSoundEffect(effectID);
    data->soundData = sound_data;
    data->onLoop = loopable;
    data->state = SOUND_INVALID;
    data->type = type;
    data->masterVolume = 1.f;
    data->leftVolume = 1.f;
    data->rightVolume = 1.f;
    
    object->soundEffectID = effectID;
    object->soundRadius = 100.f;
    
    if(!stream) {
    
        PaStreamParameters outputParameters;
        PaError err;
        
        err = Pa_Initialize();
        if(errorHandler(err)) return -1;
        
        outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
        if (outputParameters.device == paNoDevice) {
            fprintf(stderr,"Error: No default output device.\n");
            
            if(errorHandler(err)) return -1;
            
        }
        outputParameters.channelCount = STANDARD_CHANNEL_COUNT; /* stereo output */
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;
        
    
        err = Pa_OpenStream(
                            &stream,
                            NULL,
                            &outputParameters,
                            sound_data.sampleRate,
                            16,
                            paNoFlag,
                            callback,
                            NULL );
        
        if(errorHandler(err)) {
            stream = NULL;
            return -1;
        }
    }
    
    return 0;
}


void terminateSounds() {
    
    PaError err = Pa_AbortStream(stream);
    errorHandler(err);
    
    for(int x = 0; x < SOUNDEFFECT_MAX; x++) {
        
        if(effects[x].soundData.buffer) free(effects[x].soundData.buffer);
    }
    
    if(stream) {
        err = Pa_CloseStream( stream );
        errorHandler(err);
    }
    
    Pa_Terminate();
}

