#include <jni.h>
#include <string>
#include "AndroidLog.h"
#include "AudioPlayer.h"

extern "C"
{
    #include <libavformat/avformat.h>
}

static JavaVM *g_javaVM = NULL;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved){
    g_javaVM = vm;
    JNIEnv *env;

    if(vm->GetEnv((void**)(&env), JNI_VERSION_1_4) != JNI_OK){
        LOGD("JNI_OnLoad fail.\n");
        return JNI_ERR;
    }

    LOGI("JNI_OnLoad OK.\n");
    return JNI_VERSION_1_4;
}

AudioPlayer *gAudioPlayer;

extern "C"
JNIEXPORT void JNICALL
Java_cn_cnr_player_CNAudioPlayer_create_1audio(JNIEnv *env, jobject instance) {
    LOGD("new audioPlayer.");
    gAudioPlayer = new AudioPlayer(g_javaVM);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_cnr_player_CNAudioPlayer_set_1source(JNIEnv *env, jobject instance, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, JNI_FALSE);
    gAudioPlayer->setSource(path);
    env->ReleaseStringUTFChars(path_, path);
}

/*this fun will create thread download avPacket*/
extern "C"
JNIEXPORT void JNICALL
Java_cn_cnr_player_CNAudioPlayer_set_1prepared(JNIEnv *env, jobject instance) {
    gAudioPlayer->setPrepared();
}

/*set onPrepared listener, this listener will call java level listener, return some thing about audio information. */
extern "C"
JNIEXPORT void JNICALL
Java_cn_cnr_player_CNAudioPlayer_set_1onprepared_1listener(JNIEnv *env, jobject instance, jobject listener) {
    jclass  jlz = env->GetObjectClass(listener);
    if(!jlz){
        LOGD("get onPrepared listener class fail !");
        gAudioPlayer->onError("get onPrepared listener error !", -1);
        return;
    }

    jmethodID j = env->GetMethodID(jlz, "onPrepared", "(Ljava/lang/String;)V");
    if(!j){
        LOGD("get onPrepared listener method fail !");
        gAudioPlayer->onError("get onPrepared listener method fail !", -1);
        return;
    }

    gAudioPlayer->setOnPreparedListener(j, env->NewGlobalRef(listener));
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_cnr_player_CNAudioPlayer_set_1onbufferupdate_1listener(JNIEnv *env, jobject instance, jobject listener) {
    jclass  jlz = env->GetObjectClass(listener);
    if(!jlz){
        LOGD("get onbufferupdate listener class fail !");
        gAudioPlayer->onError("get on buffer update listener error !", -1);
        return;
    }

    jmethodID j = env->GetMethodID(jlz, "onBufferUpdate", "(Ljava/lang/String;)V");
    if(!j){
        LOGD("get onBufferUpdate listener method fail !");
        gAudioPlayer->onError("get onPrepared listener method fail !", -1);
        return;
    }

    gAudioPlayer->setOnBufferUpdateListener(j, env->NewGlobalRef(listener));
}


/*set onError listener, this listener will call java level listener, return error information*/
extern "C"
JNIEXPORT void JNICALL
Java_cn_cnr_player_CNAudioPlayer_set_1onerror_1listener(JNIEnv *env, jobject instance, jobject listener) {
    jclass  jlz = env->GetObjectClass(listener);
    if(!jlz){
        LOGD("get onError listener class fail !");
        gAudioPlayer->onError("get onError listener error !", -1);
        return;
    }

    jmethodID j = env->GetMethodID(jlz, "onError", "(Ljava/lang/String;I)V");
    if(!j){
        LOGD("get onError listener method fail !");
        gAudioPlayer->onError("get onError listener method fail !", -1);
        return;
    }

    gAudioPlayer->setOnErrorListener(j, env->NewGlobalRef(listener));
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_cnr_player_CNAudioPlayer_set_1metadata_1listener(JNIEnv *env, jobject instance, jobject listener) {
    jclass  jlz = env->GetObjectClass(listener);
    if(!jlz){
        LOGD("get metadata listener class fail !");
        gAudioPlayer->onError("get metadata listener error !", -1);
        return;
    }

    jmethodID j = env->GetMethodID(jlz, "onMetaData", "(Ljava/lang/String;Ljava/lang/String;)V");
    if(!j){
        LOGD("get metadata listener method fail !");
        gAudioPlayer->onError("get metadata listener method fail !", -1);
        return;
    }

    gAudioPlayer->setMetaDataListener(j, env->NewGlobalRef(listener));
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_cnr_player_CNAudioPlayer_set_1base_1info_1listener(JNIEnv *env, jobject instance, jobject listener) {
    jclass  jlz = env->GetObjectClass(listener);
    if(!jlz){
        LOGD("get base info listener class fail !");
        gAudioPlayer->onError("get base info listener error !", -1);
        return;
    }

    jmethodID j = env->GetMethodID(jlz, "onBaseInfo", "(Ljava/lang/String;Ljava/lang/String;)V");
    if(!j){
        LOGD("get base info listener method fail !");
        gAudioPlayer->onError("get base info listener method fail !", -1);
        return;
    }

    gAudioPlayer->setBaseInfoListener(j, env->NewGlobalRef(listener));
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_cnr_player_CNAudioPlayer_set_1get_1pic_1listener(JNIEnv *env, jobject instance, jobject listener, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    jclass  jlz = env->GetObjectClass(listener);
    if(!jlz){
        LOGD("get pic listener class fail !");
        gAudioPlayer->onError("get pic listener error !", -1);
        return;
    }

    jmethodID j = env->GetMethodID(jlz, "onGetPic", "(Ljava/lang/String;)V");
    if(!j){
        LOGD("get pic listener method fail !");
        gAudioPlayer->onError("get pic  listener method fail !", -1);
        return;
    }

    gAudioPlayer->setGetPicListener(j, env->NewGlobalRef(listener), path);

    env->ReleaseStringUTFChars(path_, path);
}

extern "C"
JNIEXPORT jint JNICALL
Java_cn_cnr_player_CNAudioPlayer_get_1status(JNIEnv *env, jobject instance) {

    return gAudioPlayer->getStatus();

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_cnr_player_CNAudioPlayer_start__II(JNIEnv *env, jobject instance, jint sampleRate, jint bufSize) {
    gAudioPlayer->start(sampleRate, bufSize);
}