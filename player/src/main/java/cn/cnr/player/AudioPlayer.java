package cn.cnr.player;

public interface AudioPlayer {
    void start();
    void stop();
    void setPrepared();
    void release();
    void seek(int sec);
    void setSource(String path);
    void setLooping(boolean looping);

    String getStatus();

    void setOnPreparedListener(OnPreparedListener listener);
    void setOnErrorListener(OnErrorListener listener);
    void setMetadataListener(OnMetadataListener listener);
    void setBaseInfoListener(OnBaseInfoListener listener);
    void setGetPicListener(OnGetPicListener listener, String path);
    void setBufferUpdateListener(OnBufferUpdateListener listener);

    interface OnBufferUpdateListener{
        void onBufferUpdate(String update);
    }

    interface OnGetPicListener{
        void onGetPic(String path);
    }

    interface OnBaseInfoListener{
        void onBaseInfo(String key, String value);
    }

    interface OnPreparedListener {
        void onPrepared(String s);
    }

    interface OnErrorListener{
        void onError(String s, int errorCode);
    }

    interface OnMetadataListener{
        void onMetaData(String key, String value);
    }
}
