//
// Created by king on 18-8-9.
//

#ifndef GITHUBMEDIAPLAYER_OPENSLPLAYER_H
#define GITHUBMEDIAPLAYER_OPENSLPLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "AndroidLog.h"
#include <string>
#include <assert.h>
#include "jni.h"

class OpenSLPlayer{
private:

    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine;
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    // aux effect on the output mix, used by the buffer queue player
    const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    SLmilliHertz bqPlayerSampleRate = 0;
    jint   bqPlayerBufSize = 0;
    SLObjectItf bqPlayerObject = NULL;
    SLPlayItf bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    SLEffectSendItf bqPlayerEffectSend;
    SLVolumeItf bqPlayerVolume;

// this callback handler is called every time a buffer finishes playing
    void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

public:
    SLresult createEngine();
    SLresult createBufferQueue(int sampleRate, int bufSize);
    void release();
};

#endif //GITHUBMEDIAPLAYER_OPENSLPLAYER_H
