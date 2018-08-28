package cn.cnr.player;

import android.content.Context;

public interface AudioPlayer {
    void setPrepared(String source);
    void start(Context context);
    void pause();
    void stop();
    void resume();
    void release();
    void seek(int sec);

    String getStatus();
    int getStatusInt();

    void setOnPreparedListener(OnPreparedListener listener);
    void setOnErrorListener(OnErrorListener listener);
    void setMetadataListener(OnMetadataListener listener);
    void setBaseInfoListener(OnBaseInfoListener listener);
    void setGetPicListener(OnGetPicListener listener, String path);
    void setBufferUpdateListener(OnBufferUpdateListener listener);
    void setPlayProgressingListener(OnPlayProgressing listener);
    void setOnCompletionListener(OnCompletionListener listener);

    interface OnCompletionListener{
        void onCompletion();
    }

    interface OnPlayProgressing{
        void onPlayProgressing(String update);
    }

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
