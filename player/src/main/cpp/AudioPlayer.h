//
// Created by king on 18-7-18.
//

#ifndef MEDIAPLAYER_AUDIOPLAYER_H
#define MEDIAPLAYER_AUDIOPLAYER_H


#include <string>
#include <jni.h>
#include <pthread.h>
#include "AudioQueue.h"

extern "C"
{
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};

class AudioPlayer {

private:
    enum Status {
        AUDIO_CREATE = 0,
        AUDIO_PREPARED = 1,
        AUDIO_PLAYING = 2,
        AUDIO_PAUSE = 3,
        AUDIO_STOP = 4
    };

private:
    char* audio_path = NULL;
    char* picPath;

    Status playStatus;
    double timeBase;

    int streamIndex;

    JavaVM *g_javaVM = NULL;
    jmethodID jmidOnparpared;
    jmethodID jmidOnError = NULL;
    jmethodID jmidMetadata = NULL;
    jmethodID jmidBaseInfo = NULL;
    jmethodID jmidGetPic = NULL;
    jmethodID jmidBufferUpdate = NULL;
    jmethodID jmidPlayProgressing = NULL;

    jobject objOnError = NULL;
    jobject objOnPrepared = NULL;
    jobject objMetaData = NULL;
    jobject objBaseInfo = NULL;
    jobject objGetPic = NULL;
    jobject objBufferUpdate = NULL;
    jobject objPlayProgressing = NULL;

    AVFormatContext *pFormatCtx = NULL;
    AVCodec *avCodec = NULL;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *avCodecParameters = NULL;

    AudioQueue* audioQueue;

    /*PCM*/
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine;
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    // aux effect on the output mix, used by the buffer queue player
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    SLmilliHertz bqPlayerSampleRate = 0;
    jint   bqPlayerBufSize = 0;
    SLObjectItf bqPlayerObject = NULL;
    SLPlayItf bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    SLEffectSendItf bqPlayerEffectSend;
    SLVolumeItf bqPlayerVolume;

    SwrContext *swr_ctx;
    void initSWR();
    uint8_t *outputBuffer;
    int nextSize;

    double audioDuration;
    double bufferUpdateDur;
    double playTimeStamp = 0;
    double lastPlayTimeStamp;

public:
    pthread_t thread_t;
    AudioPlayer(JavaVM *g_javaVM);
    ~AudioPlayer();

    void stop();
    void start(int sampleRate, int bufSize);
    void setOnPreparedListener(jmethodID listener, jobject obj);
    void setOnErrorListener(jmethodID listener, jobject obj);
    void setMetaDataListener(jmethodID listener, jobject obj);
    void setBaseInfoListener(jmethodID listener, jobject obj);
    void setGetPicListener(jmethodID listener, jobject obj, const char* path);
    void setOnBufferUpdateListener(jmethodID listener, jobject obj);
    void setOnPlayProgressing(jmethodID listener, jobject obj);

    void setPrepared();
    void setSource(const char* path);

    void onError(const char* s, int errorCode);
    void onPrepared(const char* s);
    void onGetMetaData(const char* key, const char* value);
    void onBaseInfo(const char* key, const char* value);
    void onGetPic(const char* path);
    void onBufferUpdate(const char* s);
    void onPlayProgressing(const char *s);

    void prepared_fun();

    void setStatus(Status status);
    int getStatus();

    /*PCM*/
    SLresult createEngine();
    SLresult createBufferQueue(int sampleRate, int bufSize);
    void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
};


#endif //MEDIAPLAYER_AUDIOPLAYER_H
