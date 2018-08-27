package cn.cnr.player;

import android.content.Context;

public class CNPlayer {
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("ffmpeg");
    }

    private static CNAudioPlayer audioPlayerInstance = null;

    public static AudioPlayer getAudioPlayerInstance(Context context){
        if(audioPlayerInstance == null){
            audioPlayerInstance = new CNAudioPlayer(context);
        }

        return audioPlayerInstance;
    }

    public VideoPlayer getVodioPlayerInstance(){
        return null;
    }

}
