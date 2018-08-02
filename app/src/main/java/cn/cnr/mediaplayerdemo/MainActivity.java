package cn.cnr.mediaplayerdemo;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import cn.cnr.mediaplayer.R;
import cn.cnr.player.AudioPlayer;
import cn.cnr.player.CNPlayer;
import cn.cnr.player.CNTrace;

public class MainActivity extends AppCompatActivity implements AudioPlayer.OnPreparedListener, AudioPlayer.OnErrorListener,
    AudioPlayer.OnMetadataListener, AudioPlayer.OnBaseInfoListener{

    private AudioPlayer player = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        player = CNPlayer.getAudioPlayerInstance();
        player.setOnErrorListener(this);
        player.setOnPreparedListener(this);
        player.setMetadataListener(this);
        player.setBaseInfoListener(this);
    }

    public void audio_prepared(View view) {
        player.setPrepared();
    }

    public void audio_start(View view) {
        player.start();
    }

    public void audio_setsource(View view) {
        player.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
    }

    @Override
    public void onPrepared(String s) {
        CNTrace.d("onPrepared : " + s);
    }

    @Override
    public void onError(String s, int errorCode) {
        CNTrace.d("error : " + s + ", errorCode : " + errorCode);
    }

    @Override
    public void onMetaData(String key, String value) {
        CNTrace.d("MetaData key : " + key + ", value : " + value);
    }

    @Override
    public void onBaseInfo(String key, String value) {
        CNTrace.d("onBaseInfo key : " + key + ", value : " + value);
    }
}
