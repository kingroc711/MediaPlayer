package cn.cnr.player;

class CNAudioPlayer implements AudioPlayer {
    private native void create_audio();
    private native void set_source(String path);
    private native void set_prepared();
    private native void set_onprepared_listener(OnPreparedListener listener);
    private native void set_onerror_listener(OnErrorListener listener);
    private native void set_metadata_listener(OnMetadataListener listener);
    private native void set_base_info_listener(OnBaseInfoListener listener);

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
}