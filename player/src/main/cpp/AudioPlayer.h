//
// Created by king on 18-7-18.
//

#ifndef MEDIAPLAYER_AUDIOPLAYER_H
#define MEDIAPLAYER_AUDIOPLAYER_H


#include <string>
#include <jni.h>
#include <pthread.h>

extern "C"
{
#include "libavformat/avformat.h"
};

using  std::string;

class AudioPlayer {
private:
    char* path;
    char* picPath;

    JavaVM *g_javaVM = NULL;
    jmethodID jmidOnparpared;
    jmethodID jmidOnError = NULL;
    jmethodID jmidMetadata = NULL;
    jmethodID jmidBaseInfo = NULL;
    jmethodID jmidGetPic = NULL;

    jobject objOnError = NULL;
    jobject objOnPrepared = NULL;
    jobject objMetaData = NULL;
    jobject objBaseInfo = NULL;
    jobject objGetPic = NULL;


    AVFormatContext *pFormatCtx = NULL;
    AVCodec *avCodec = NULL;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *avCodecParameters = NULL;

public:
    pthread_t thread_t;
    AudioPlayer(JavaVM *g_javaVM);
    ~AudioPlayer();

    void start();
    void setOnPreparedListener(jmethodID listener, jobject obj);
    void setOnErrorListener(jmethodID listener, jobject obj);
    void setMetaDataListener(jmethodID listener, jobject obj);
    void setBaseInfoListener(jmethodID listener, jobject obj);
    void setGetPicListener(jmethodID listener, jobject obj, const char* path);

    void setPrepared();
    void setSource(const char* path);

    void onError(const char* s, int errorCode);
    void onPrepared(const char* s);
    void onGetMetaData(const char* key, const char* value);
    void onBaseInfo(const char* key, const char* value);
    void onGetPic(const char* path);

    void prepared_fun();
};


#endif //MEDIAPLAYER_AUDIOPLAYER_H
