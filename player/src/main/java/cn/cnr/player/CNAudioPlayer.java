package cn.cnr.player;

class CNAudioPlayer implements AudioPlayer {
    private native void create_audio();
    private native void set_source(String path);
    private native void set_prepared();
    private native void set_onprepared_listener(OnPreparedListener listener);
    private native void set_onerror_listener(OnErrorListener listener);
    private native void set_metadata_listener(OnMetadataListener listener);
    private native void set_base_info_listener(OnBaseInfoListener listener);
    private native void set_get_pic_listener(OnGetPicListener listener, String path);
    private native int get_status();
    private native void set_onbufferupdate_listener(OnBufferUpdateListener listener);

    public CNAudioPlayer(){
        create_audio();
    }

    @Override
    public void start() {

    }

    @Override
    public void stop() {

    }

    @Override
    public void setPrepared() {
        set_prepared();
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
    public void release() {

    }

    @Override
    public void seek(int sec) {

    }

    @Override
    public void setSource(String path) {
        set_source(path);
    }

    @Override
    public void setLooping(boolean looping) {

    }

    @Override
    public String getStatus() {
        int status = get_status();
        CNTrace.d("get audio status : " + status);

        switch (status){
            case 0:
                return "AUDIO_CREATE";
            case 1:
                return "AUDIO_PREPARED";
            case 2:
                return "AUDIO_START";
            case 3:
                return "AUDIO_PAUSE";
            case 4:
                return "AUDIO_STOP";
                default:
                    return "UNKNOW";
        }
    }
}