//
// Created by king on 18-7-18.
//

#ifndef MEDIAPLAYER_AUDIOPLAYER_H
#define MEDIAPLAYER_AUDIOPLAYER_H


#include <string>
#include <jni.h>
#include <pthread.h>
#include "AudioQueue.h"
#include "OpenSLPlayer.h"

extern "C"
{
#include "libavformat/avformat.h"
};

class AudioPlayer {

private:
    enum Status {
        AUDIO_CREATE = 0,
        AUDIO_PREPARED = 1,
        AUDIO_START = 2,
        AUDIO_PAUSE = 3,
        AUDIO_STOP = 4
    };

private:
    char* path;
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

    jobject objOnError = NULL;
    jobject objOnPrepared = NULL;
    jobject objMetaData = NULL;
    jobject objBaseInfo = NULL;
    jobject objGetPic = NULL;
    jobject objBufferUpdate = NULL;

    AVFormatContext *pFormatCtx = NULL;
    AVCodec *avCodec = NULL;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *avCodecParameters = NULL;

    AudioQueue* audioQueue;
    OpenSLPlayer* openSLPlayer;

public:
    pthread_t thread_t;
    AudioPlayer(JavaVM *g_javaVM);
    ~AudioPlayer();

    void start(int sampleRate, int bufSize);
    void setOnPreparedListener(jmethodID listener, jobject obj);
    void setOnErrorListener(jmethodID listener, jobject obj);
    void setMetaDataListener(jmethodID listener, jobject obj);
    void setBaseInfoListener(jmethodID listener, jobject obj);
    void setGetPicListener(jmethodID listener, jobject obj, const char* path);
    void setOnBufferUpdateListener(jmethodID listener, jobject obj);

    void setPrepared();
    void setSource(const char* path);

    void onError(const char* s, int errorCode);
    void onPrepared(const char* s);
    void onGetMetaData(const char* key, const char* value);
    void onBaseInfo(const char* key, const char* value);
    void onGetPic(const char* path);
    void onBufferUpdate(const char* s);

    void prepared_fun();

    void setStatus(Status status);
    int getStatus();
};


#endif //MEDIAPLAYER_AUDIOPLAYER_H
