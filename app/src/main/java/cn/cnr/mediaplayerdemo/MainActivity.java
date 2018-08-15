package cn.cnr.mediaplayerdemo;

import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;

import java.io.File;

import cn.cnr.mediaplayer.R;
import cn.cnr.player.AudioPlayer;
import cn.cnr.player.CNPlayer;
import cn.cnr.player.CNTrace;

public class MainActivity extends AppCompatActivity implements AudioPlayer.OnPreparedListener, AudioPlayer.OnErrorListener,
    AudioPlayer.OnMetadataListener, AudioPlayer.OnBaseInfoListener, AudioPlayer.OnGetPicListener, AudioPlayer.OnBufferUpdateListener,
AudioPlayer.onPlayProgressing{

    private AudioPlayer player = null;
    private ImageView imageView;

    Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 1:
                    CNTrace.d("get pic path : " + msg.obj);
                    imageView.setImageURI(Uri.fromFile(new File((String)msg.obj)));
                    break;

                default:
                    break;
            }
        };
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imageView = (ImageView)findViewById(R.id.b_pic);

        player = CNPlayer.getAudioPlayerInstance();
        CNTrace.d("player statue : " + player.getStatus());

        player.setOnErrorListener(this);
        player.setOnPreparedListener(this);
        player.setMetadataListener(this);
        player.setBaseInfoListener(this);
        player.setGetPicListener(this, this.getCacheDir().toString());
        player.setBufferUpdateListener(this);
        player.setPlayProgressing(this);
    }

    public void audio_prepared(View view) {
        player.setPrepared();
    }

    public void audio_start(View view) {
        player.start(this);
    }

    public void audio_setsource(View view) {
        player.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
    }

    @Override
    public void onPrepared(String s) {
        CNTrace.d("onPrepared : " + s);
        CNTrace.d("player statue : " + player.getStatus());
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

    @Override
    public void onGetPic(String path) {
        CNTrace.d("pic path : " + path);
        String pic_path = new String(path);

        Message msg = mHandler.obtainMessage();
        msg.what = 1;
        msg.obj = pic_path;
        mHandler.sendMessage(msg);
    }

    @Override
    public void onBufferUpdate(String update) {
        CNTrace.d(update);
    }

    @Override
    public void onPlayProgressing(String update) {
        CNTrace.d(update);
    }
}
