//
// Created by king on 18-8-9.
//

#include "OpenSLPlayer.h"

void OpenSLPlayer::release() {
}

SLresult OpenSLPlayer::createEngine() {
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if(result != SL_RESULT_SUCCESS) {
        LOGD("create engine result : %d\n", result);
        return result;
    }

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS) {
        LOGD("realize engine result : %d\n", result);
        return result;
    }

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if(result != SL_RESULT_SUCCESS) {
        LOGD("interface engine result : %d\n", result);
        return result;
    }

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    if(result != SL_RESULT_SUCCESS) {
        LOGD("create output mix result : %d\n", result);
        return result;
    }

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS) {
        LOGD("realize outputmixobject result : %d\n", result);
        return result;
    }

    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    LOGD("get interface outputmixobject result : %d\n", result);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &reverbSettings);
        LOGD("SetEnvironmentalReverbProperties result : %d\n", result);
        return result;
    }
    // ignore unsuccessful result codes for environmental reverb, as it is optional for this example


    return !SL_RESULT_SUCCESS;
}

SLresult OpenSLPlayer::createBufferQueue(int sampleRate, int bufSize) {
    SLresult result;
    if (sampleRate >= 0 && bufSize >= 0 ) {
        this->bqPlayerSampleRate = sampleRate * 1000;
        /*
         * device native buffer size is another factor to minimize audio latency, not used in this
         * sample: we only play one giant buffer here
         */
        this->bqPlayerBufSize = bufSize;
    }

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_8,
                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};

    /*
     * Enable Fast Audio when possible:  once we set the same rate to be the native, fast audio path
     * will be triggered
     */
    if(this->bqPlayerSampleRate) {
        format_pcm.samplesPerSec = this->bqPlayerSampleRate;       //sample rate in mili second
    }


    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, this->outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    /*
    * create audio player:
    *     fast audio does not support when SL_IID_EFFECTSEND is required, skip it
    *     for fast audio case
    */
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ };

    result = (*this->engineEngine)->CreateAudioPlayer(this->engineEngine, &this->bqPlayerObject, &audioSrc, &audioSnk,
                                                this->bqPlayerSampleRate? 2 : 3, ids, req);
    if(SL_RESULT_SUCCESS != result){
        LOGE("create audio player error !");
        return result;
    }

    // realize the player
    result = (*this->bqPlayerObject)->Realize(this->bqPlayerObject, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS){
        LOGE("player realize error !");
        return result;
    }

    // get the play interface
    result = (*this->bqPlayerObject)->GetInterface(this->bqPlayerObject, SL_IID_PLAY, &this->bqPlayerPlay);
    if(SL_RESULT_SUCCESS != result){
        LOGE("player get interface");
        return  result;
    }

    // get the buffer queue interface
    result = (*this->bqPlayerObject)->GetInterface(this->bqPlayerObject, SL_IID_BUFFERQUEUE, &this->bqPlayerBufferQueue);
    if(SL_RESULT_SUCCESS != result){
        LOGE("get player buffer Queue");
        return result;
    }

    // register callback on the buffer queue
    result = (*this->bqPlayerBufferQueue)->RegisterCallback(this->bqPlayerBufferQueue, this->bqPlayerCallback, NULL);
    if(SL_RESULT_SUCCESS != result){
        LOGE("register callback error!");
        return result;
    }

    // get the effect send interface
    this->bqPlayerEffectSend = NULL;
    if( 0 == this->bqPlayerSampleRate) {
        result = (*this->bqPlayerObject)->GetInterface(this->bqPlayerObject, SL_IID_EFFECTSEND, &this->bqPlayerEffectSend);
        if(SL_RESULT_SUCCESS != result){
            LOGE("get player object interfacer error !");
            return result;
        }
    }

#if 0   // mute/solo is not supported for sources that are known to be mono, as this is
    // get the mute/solo interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_MUTESOLO, &bqPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
#endif

    // get the volume interface
    result = (*this->bqPlayerObject)->GetInterface(this->bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    if(SL_RESULT_SUCCESS != result){
        LOGE("player object get interface");
        return result;
    }

    // set the player's state to playing
    result = (*this->bqPlayerPlay)->SetPlayState(this->bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    if(SL_RESULT_SUCCESS != result){
        LOGE("set the player's state to playing");
        return result;
    }

    return SL_RESULT_SUCCESS;
}

// this callback handler is called every time a buffer finishes playing
void OpenSLPlayer::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
//    assert(bq == bqPlayerBufferQueue);
//    assert(NULL == context);
//    // for streaming playback, replace this test by logic to find and fill the next buffer
//    if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize) {
//        SLresult result;
//        // enqueue another buffer
//        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
//        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
//        // which for this code example would indicate a programming error
//        if (SL_RESULT_SUCCESS != result) {
//            pthread_mutex_unlock(&audioEngineLock);
//        }
//        (void)result;
//    } else {
//        releaseResampleBuf();
//        pthread_mutex_unlock(&audioEngineLock);
//    }
}

