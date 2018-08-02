package cn.cnr.player;

public class CNPlayer {
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("ffmpeg");
    }

    private static CNAudioPlayer audioPlayerInstance = null;

    public static AudioPlayer getAudioPlayerInstance(){
        if(audioPlayerInstance == null){
            audioPlayerInstance = new CNAudioPlayer();
        }

        return audioPlayerInstance;
    }

    public VideoPlayer getVodioPlayerInstance(){
        return null;
    }

}
