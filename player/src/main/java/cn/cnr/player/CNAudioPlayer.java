package cn.cnr.player;

import android.content.Context;
import android.media.AudioManager;
import android.os.Build;

class CNAudioPlayer implements AudioPlayer {
    private native void create_audio(int sampleRate, int bufSize);
    private native void set_prepared(String source);
    private native void set_onprepared_listener(OnPreparedListener listener);
    private native void set_onerror_listener(OnErrorListener listener);
    private native void set_metadata_listener(OnMetadataListener listener);
    private native void set_base_info_listener(OnBaseInfoListener listener);
    private native void set_get_pic_listener(OnGetPicListener listener, String path);
    private native int  get_status();
    private native void set_onbufferupdate_listener(OnBufferUpdateListener listener);
    private native void set_start();
    private native void set_stop();
    private native void set_pause();
    private native void set_onPlayProgressing_listener(onPlayProgressing listener);

    public CNAudioPlayer(Context context){
        int sampleRate = 0;
        int bufSize = 0;

        /*
         * retrieve fast audio path sample rate and buf size; if we have it, we pass to native
         * side to create a player with fast audio enabled [ fast audio == low latency audio ];
         * IF we do not have a fast audio path, we pass 0 for sampleRate, which will force native
         * side to pick up the 8Khz sample rate.
         */
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            AudioManager myAudioMgr = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
            String nativeParam = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
            sampleRate = Integer.parseInt(nativeParam);
            nativeParam = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
            bufSize = Integer.parseInt(nativeParam);
        }

        create_audio(sampleRate, bufSize);
    }

    @Override
    public void start(Context context) {
        set_start();
    }

    @Override
    public void pause() {
        set_pause();
    }

    @Override
    public void stop() {
        set_stop();
    }

    @Override
    public void setPrepared(String source) {
        set_prepared(source);
    }

    @Override
    public void setOnPreparedListener(OnPreparedListener listener) {
        set_onprepared_listener(listener);
    }

    @Override
    public void setOnErrorListener(OnErrorListener listener) {
        set_onerror_listener(listener);
    }

    @Override
    public void setMetadataListener(OnMetadataListener listener) {
        set_metadata_listener(listener);
    }

    @Override
    public void setBaseInfoListener(OnBaseInfoListener listener) {
        set_base_info_listener(listener);
    }

    @Override
    public void setGetPicListener(OnGetPicListener listener, String path) {
        set_get_pic_listener(listener, path);
    }

    @Override
    public void setBufferUpdateListener(OnBufferUpdateListener listener) {
        set_onbufferupdate_listener(listener);
    }

    @Override
    public void setPlayProgressing(onPlayProgressing listener) {
        set_onPlayProgressing_listener(listener);
    }

    @Override
    public void release() {

    }

    @Override
    public void seek(int sec) {

    }

    @Override
    public void setLooping(boolean looping) {

    }

    @Override
    public String getStatus() {
        int status = get_status();
        switch (status){
            case 0:
                return "AUDIO_CREATE";
            case 1:
                return "AUDIO_TOPREPARE";
            case 2:
                return "AUDIO_PREPARED";
            case 3:
                return "AUDIO_PLAYING";
            case 4:
                return "AUDIO_PAUSE";
            case 5:
                return "AUDIO_STOP";
                default:
                    return "UNKNOW";
        }
    }

    @Override
    public int getStatusInt() {
        return get_status();
    }
}