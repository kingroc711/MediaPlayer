#include <unistd.h>
#include "AudioPlayer.h"
#include "AndroidLog.h"

void* toPrepared(void* data){
    AudioPlayer *player = (AudioPlayer*)data;
    player->prepared_fun();
    pthread_detach(pthread_self());
    pthread_exit(&player->thread_t);
}

void AudioPlayer::setPrepared(const char* source) {
    int len = strlen(source);
    this->audio_path = (char*)malloc(len + 1);
    strcpy(this->audio_path, source);
    LOGD("set source path %s\n", this->audio_path);

    pthread_create(&this->thread_t, NULL, toPrepared, this);
}

AudioPlayer::AudioPlayer(JavaVM *g_javaVM) {
    this->g_javaVM = g_javaVM;
    this->setStatus(AUDIO_CREATE);
    this->audioQueue = new AudioQueue();
}

AudioPlayer::~AudioPlayer() {
    free (this->audio_path);
    this->audio_path = NULL;
    free (this->picPath);

    delete(this->audioQueue);

    unlink(this->picPath);

    if(this->avCodecContext){
        avcodec_close(this->avCodecContext);
        av_free(this->avCodecContext);
    }

    if(this->pFormatCtx){
        avformat_close_input(&this->pFormatCtx);
    }

    JNIEnv *env;
    this->g_javaVM->AttachCurrentThread(&env, NULL);
    if(this->jmidMetadata)
        env->DeleteGlobalRef(this->objMetaData);

    if(this->jmidOnError)
        env->DeleteGlobalRef(this->objOnError);

    if(this->jmidOnparpared)
        env->DeleteGlobalRef(this->objOnPrepared);

    if(this->jmidBaseInfo)
        env->DeleteGlobalRef(this->objBaseInfo);

    if(this->jmidBufferUpdate)
        env->DeleteGlobalRef(this->objBufferUpdate);

    if(this->jmidPlayProgressing)
        env->DeleteGlobalRef(this->objPlayProgressing);

    this->g_javaVM->DetachCurrentThread();
}

int AudioPlayer::getStatus() {
    return this->playStatus;
}

void AudioPlayer::setStatus(AudioPlayer::Status status) {
    this->playStatus = status;
}

void AudioPlayer::setOnBufferUpdateListener(jmethodID listener, jobject obj) {
    LOGD("jni set on buffer update listener. \n");
    this->jmidBufferUpdate = listener;
    this->objBufferUpdate = obj;
}

void AudioPlayer::setGetPicListener(jmethodID listener, jobject obj, const char* path) {
    LOGD("jni set get pic listener.\n");
    this->jmidGetPic = listener;
    this->objGetPic = obj;

    this->picPath = (char*)malloc(strlen(path) + 1);
    strcpy(this->picPath, path);
}


void AudioPlayer::setBaseInfoListener(jmethodID listener, jobject obj) {
    LOGD("jni set on base info listener.\n");
    this->jmidBaseInfo = listener;
    this->objBaseInfo = obj;
}

void AudioPlayer::setOnPlayProgressing(jmethodID listener, jobject obj) {
    LOGD("jni set on play progressing listener.\n");
    this->jmidPlayProgressing = listener;
    this->objPlayProgressing = obj;
}

void AudioPlayer::setOnErrorListener(jmethodID listener, jobject obj) {
    LOGD("jni set on error listener.\n");
    this->jmidOnError = listener;
    this->objOnError = obj;
}

void AudioPlayer::setOnPreparedListener(jmethodID listener, jobject obj) {
    LOGD("jni set on prepared listener.\n");
    this->jmidOnparpared = listener;
    this->objOnPrepared = obj;
}

void AudioPlayer::setMetaDataListener(jmethodID listener, jobject obj) {
    LOGD("jni set metaData listener.\n");
    this->jmidMetadata = listener;
    this->objMetaData = obj;
}

void AudioPlayer::onBufferUpdate(const char *s) {
    if(this->jmidBufferUpdate){
        JNIEnv *env;
        int getEnvStat = this->g_javaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->AttachCurrentThread(&env, NULL);

        jstring ss = env->NewStringUTF(s);
        env->CallVoidMethod(this->objBufferUpdate, this->jmidBufferUpdate, ss);
        env->DeleteLocalRef(ss);

        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->DetachCurrentThread();
    }
}

void AudioPlayer::onGetPic(const char* path) {
    if(this->jmidGetPic){
        JNIEnv *env;
        int getEnvStat = this->g_javaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->AttachCurrentThread(&env, NULL);

        jstring s = env->NewStringUTF(path);
        env->CallVoidMethod(this->objGetPic, this->jmidGetPic, s);
        env->DeleteLocalRef(s);

        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->DetachCurrentThread();
    }
}

void AudioPlayer::onBaseInfo(const char* key, const char* value){
    if(this->jmidBaseInfo){
        JNIEnv *env;
        int getEnvStat = this->g_javaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->AttachCurrentThread(&env, NULL);

        jstring k = env->NewStringUTF(key);
        jstring v = env->NewStringUTF(value);
        env->CallVoidMethod(this->objBaseInfo, this->jmidBaseInfo, k, v);
        env->DeleteLocalRef(k);
        env->DeleteLocalRef(v);

        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->DetachCurrentThread();
    }
}

void AudioPlayer::onPrepared(const char *s) {
    if(this->jmidOnparpared){
        JNIEnv *env;
        int getEnvStat = this->g_javaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->AttachCurrentThread(&env, NULL);

        jstring str_arg = env->NewStringUTF(s);
        env->CallVoidMethod(this->objOnPrepared, this->jmidOnparpared, str_arg);
        env->DeleteLocalRef(str_arg);

        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->DetachCurrentThread();
    }
}

void AudioPlayer::onPlayProgressing(const char *s) {
    if(this->jmidPlayProgressing){
        JNIEnv *env;
        int getEnvStat = this->g_javaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
        //LOGD("get Evn Stat : %d\n", getEnvStat);

        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->AttachCurrentThread(&env, NULL);

        jstring str_arg = env->NewStringUTF(s);
        env->CallVoidMethod(this->objPlayProgressing, this->jmidPlayProgressing, str_arg);
        env->DeleteLocalRef(str_arg);

        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->DetachCurrentThread();
    }
}

void AudioPlayer::onError(const char* s, int errorCode) {
    if(this->jmidOnError){
        JNIEnv *env;
        int getEnvStat = this->g_javaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
        //LOGD("get Evn Stat : %d\n", getEnvStat);

        if(getEnvStat == JNI_EDETACHED){
            this->g_javaVM->AttachCurrentThread(&env, NULL);
        }

        jstring str_arg = env->NewStringUTF(s);
        //env->CallObjectMethod(this->objOnError, this->jmidOnError, str_arg, errorCode);
        env->CallVoidMethod(this->objOnError, this->jmidOnError, str_arg, errorCode);
        env->DeleteLocalRef(str_arg);

        if(getEnvStat == JNI_EDETACHED) {
            this->g_javaVM->DetachCurrentThread();
        }
    }
}

void AudioPlayer::onGetMetaData(const char *key, const char *value) {
    if(this->jmidMetadata){
        JNIEnv *env;
        int getEnvStat = this->g_javaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
        //LOGD("get Evn Stat : %d\n", getEnvStat);

        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->AttachCurrentThread(&env, NULL);

        jstring k = env->NewStringUTF(key);
        jstring v = env->NewStringUTF(value);
        env->CallVoidMethod(this->objMetaData, this->jmidMetadata, k, v);
        env->DeleteLocalRef(k);
        env->DeleteLocalRef(v);

        if(getEnvStat == JNI_EDETACHED)
            this->g_javaVM->DetachCurrentThread();
    }
}

void AudioPlayer::prepared_fun() {
    LOGD("thread prepared function");

    av_register_all();
    avformat_network_init();
    this->pFormatCtx = avformat_alloc_context();

    AVDictionary* format_opts = NULL;
    if (!av_dict_get(format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
        av_dict_set(&format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
    }

    if(avformat_open_input(&this->pFormatCtx, this->audio_path, NULL, &format_opts))
    {
        LOGE("can not open url :%s", this->audio_path);
        this->onError("avformat can not open url.", -1);
        return;
    }

    if(avformat_find_stream_info(this->pFormatCtx, NULL) < 0)
    {
        LOGE("can not find streams from %s", this->audio_path);
        this->onError("can not find streams from url.\n", -1);
        return;
    }

    /*get meta data sent java*/
    AVDictionaryEntry *m = NULL;
    while(m = av_dict_get(pFormatCtx->metadata, "" , m, AV_DICT_IGNORE_SUFFIX)) {
        this->onGetMetaData(m->key, m->value);
    }

    /*here get avCodeParameters and avCodec*/
    for(int i = 0; i < this->pFormatCtx->nb_streams; i++) {
        if(this->pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            LOGI("AVFormatContext nb streams : %d code_type \n", i);
            this->streamIndex = i;
            this->timeBase = av_q2d(this->pFormatCtx->streams[i]->time_base);
            this->avCodecParameters = pFormatCtx->streams[i]->codecpar;
            this->avCodec = avcodec_find_decoder(this->avCodecParameters->codec_id);
            if(!this->avCodec){
                LOGE("can not find decoder");
                this->onError("can not find decoder", -1);
                return;
            } else{
                LOGI("find audio codec\n");
                break;
            }
        }
    }

    /*here get pic from audio*/
    if(this->jmidGetPic) {
        for (int i = 0; i < pFormatCtx->nb_streams; i++) {
            if (this->pFormatCtx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
                char buf[128];
                sprintf(buf, "%s", this->picPath);
                char* e = strrchr(this->audio_path, '/');
                strcat(buf, e);
                LOGD("pic file name : %s\n", buf);
                AVPacket pkt = this->pFormatCtx->streams[i]->attached_pic;
                FILE* album_art = fopen(buf, "wb");
                fwrite(pkt.data, pkt.size, 1, album_art);
                fclose(album_art);
                this->onGetPic(buf);
                break;
            }
        }
    }

    /*get audio base info sent java*/
    char buf[128];
    this->audioDuration  = this->pFormatCtx->duration * av_q2d(AV_TIME_BASE_Q);
    this->bufferUpdateDur = this->audioDuration/20 > 1 ? this->audioDuration/20 : 1;
    sprintf(buf, "%f", this->audioDuration);
    this->onBaseInfo("duration", buf);
    this->onBaseInfo("format", this->pFormatCtx->iformat->name);

    sprintf(buf, "%lld kb/s", (int64_t)this->pFormatCtx->bit_rate / 1000);
    this->onBaseInfo("bit_rate", buf);

    sprintf(buf, "%d Hz", this->avCodecParameters->sample_rate);
    this->onBaseInfo("sample_rate", buf);

    this->avCodecContext = avcodec_alloc_context3(this->avCodec);
    if(!avCodecContext){
        LOGE("can not alloc new decodecctx");
        this->onError("can not alloc new decodecctx\n", -1);
        return;
    }

    if(avcodec_parameters_to_context(this->avCodecContext, this->avCodecParameters) < 0){
        LOGE("can not fill decodecctx");
        this->onError("avcodec_parameters_to_context", -1);
        return;
    }

    if(avcodec_open2(this->avCodecContext, this->avCodec, 0) != 0){
        LOGE("avcodec_open2");
        this->onError("avcodec_open2", -1);
        return;
    }

    LOGD("avcodec open success !\n");

    this->setStatus(AUDIO_PREPARED);
    this->onPrepared("");

    double bufferSecond = 0;
    double lastBufferSecond = 0;
    while (this->getStatus() != AUDIO_STOP){
        AVPacket *avPacket = av_packet_alloc();
        int ret = av_read_frame(this->pFormatCtx, avPacket);
        //LOGD("pts : %f, dts : %f, duration : %f\n", avPacket->pts * this->timeBase, avPacket->dts, avPacket->duration);
        if(ret == 0){
            if(avPacket->stream_index == this->streamIndex) {
                audioQueue->putAvpacket(avPacket);
                lastBufferSecond = (avPacket->pts + avPacket->duration) * this->timeBase;
                if (lastBufferSecond - bufferSecond > this->bufferUpdateDur) {
                    bufferSecond = lastBufferSecond;
                    char b[128];
                    sprintf(b, "%f", bufferSecond);
                    this->onBufferUpdate(b);
                }
            }
        }else{
            av_packet_unref(avPacket);
            av_free(avPacket);
            //LOGD("av read frame ret : %d\n", ret);
            break;
        }
    }

    if(lastBufferSecond > bufferSecond){
        char b[128];
        sprintf(b, "%f", lastBufferSecond);
        this->onBufferUpdate(b);
    }

    LOGD("get data finish thread exit !\n");
    LOGD("Package size : %d, total size : %d\n", this->audioQueue->size(), this->audioQueue->getDataSize());
    return;
}

void AudioPlayer::start(int sampleRate, int bufSize) {

    SLresult result = this->createEngine();
    if(result != SL_RESULT_SUCCESS){
        LOGE("create player error %d.", result);
        this->onError("create player error.", result);
        return;
    }
    LOGD("create player engine OK");

    result = this->createBufferQueue(sampleRate, bufSize);
    if(result != SL_RESULT_SUCCESS){
        LOGE("create buffer queue error %d\n", result);
        this->onError("\"create buffer queue error", result);
        return;
    }
    LOGD("create buffer queue OK");
    this->initSWR();

    this->setStatus(AUDIO_PLAYING);
    this->playTimeStamp = 0;

    bqPlayerCallback(this->bqPlayerBufferQueue, this);
}

void AudioPlayer::stop() {
    // set the player's state to stop
    SLresult result = (*this->bqPlayerPlay)->SetPlayState(this->bqPlayerPlay, SL_PLAYSTATE_STOPPED);
    if(SL_RESULT_SUCCESS != result){
        LOGE("set the player's state to playing");
        return;
    }
}

SLresult AudioPlayer::createEngine() {
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
        //result =
        (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &reverbSettings);
        LOGD("SetEnvironmentalReverbProperties result : %d\n", result);
        return SL_RESULT_SUCCESS;
    }
    // ignore unsuccessful result codes for environmental reverb, as it is optional for this example


    return !SL_RESULT_SUCCESS;
}

void funbqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context){
    AudioPlayer *p = (AudioPlayer*)context;
    p->bqPlayerCallback(bq, context);
}

SLresult AudioPlayer::createBufferQueue(int sampleRate, int bufSize) {
    SLresult result;

    this->outputBuffer = (uint8_t*)malloc(8192*sizeof(uint8_t));

    LOGD("sampleRate : %d, bufSize : %d\n", sampleRate, bufSize);
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
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,
                                   2,
                                   this->avCodecContext->sample_rate,
                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                                   SL_BYTEORDER_LITTLEENDIAN};

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
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND, /*SL_IID_MUTESOLO,*/};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, /*SL_BOOLEAN_TRUE,*/ };

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
    result = (*this->bqPlayerBufferQueue)->RegisterCallback(this->bqPlayerBufferQueue, funbqPlayerCallback, this);
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

void AudioPlayer::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {

    AVPacket *avPacket = NULL;
    if(this->audioQueue->getAvpacket(&avPacket) > 0){
        int ret = avcodec_send_packet(this->avCodecContext, avPacket);
        if (ret != 0){
            LOGE("avcode send packet error ret : %d\n", ret);
            char buf[128];
            sprintf(buf, "%f", this->lastPlayTimeStamp);
            this->onPlayProgressing(buf);

            av_packet_free(&avPacket);

            this->onError("avcode send packet error ret : %d", ret);
            return;
        }

        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(this->avCodecContext, frame);
        if(ret != 0){
            LOGE("avcode receive frame error ret %d", ret);
            this->onError("avcode receive frame error ret %d", ret);
            av_frame_free(&frame);
            av_packet_free(&avPacket);
            return;
        }

        this->lastPlayTimeStamp = (frame->pts + frame->pkt_duration) * this->timeBase;
        if(this->lastPlayTimeStamp - this->playTimeStamp  > this->bufferUpdateDur/4){
            this->playTimeStamp = this->lastPlayTimeStamp;
            char buf[128];
            sprintf(buf, "%f", this->lastPlayTimeStamp);
            this->onPlayProgressing(buf);
        }


        this->nextSize = av_samples_get_buffer_size(frame->linesize, this->avCodecContext->channels,
                                                    this->avCodecContext->frame_size, this->avCodecContext->sample_fmt, 1);
        ret = swr_convert(this->swr_ctx, &this->outputBuffer, frame->nb_samples,
                          (const uint8_t **) frame->data, frame->nb_samples);
        (*this->bqPlayerBufferQueue)->Enqueue(this->bqPlayerBufferQueue, this->outputBuffer, this->nextSize);
        //LOGD("swr_convert ret : %d", ret);
        av_frame_free(&frame);
        av_packet_free(&avPacket);
    }else if(this->lastPlayTimeStamp - this->playTimeStamp > 0){
        char buf[128];
        sprintf(buf, "%f", this->lastPlayTimeStamp);
        this->onPlayProgressing(buf);
    }
}

void AudioPlayer::initSWR() {
    this->swr_ctx = swr_alloc_set_opts(
            NULL,
            this->avCodecContext->channel_layout,
            AV_SAMPLE_FMT_S16,
            this->avCodecContext->sample_rate,
            this->avCodecContext->channel_layout,
            this->avCodecContext->sample_fmt,
            this->avCodecContext->sample_rate,
            0, NULL);

    if (!this->swr_ctx || swr_init(this->swr_ctx) < 0) {
        LOGE("swr init error.");
        swr_free(&this->swr_ctx);
        return;
    }

    swr_init(this->swr_ctx);
}