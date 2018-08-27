package cn.cnr.mediaplayerdemo;

import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import java.io.File;

import cn.cnr.mediaplayer.R;
import cn.cnr.player.AudioPlayer;
import cn.cnr.player.CNPlayer;
import cn.cnr.player.CNTrace;

public class TestActivity extends AppCompatActivity implements AudioPlayer.OnPreparedListener, AudioPlayer.OnErrorListener,
        AudioPlayer.OnMetadataListener, AudioPlayer.OnBaseInfoListener, AudioPlayer.OnGetPicListener, AudioPlayer.OnBufferUpdateListener,
        AudioPlayer.onPlayProgressing{

    private AudioPlayer player = null;
    private ImageView imageView;
    private EditText editText;
    private TextView  textView;
    private ProgressBar progressBar;

    private Float duration;

    Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 1:
                    imageView.setImageURI(Uri.fromFile(new File((String)msg.obj)));
                    break;

                case 2:
                    String s = textView.getText().toString();
                    s = s + "\n" + msg.obj;
                    textView.setText(s);
                    break;

                case 3: {
                    float stp = Float.valueOf((String) msg.obj);
                    int precent = (int) (stp / duration * 100);
                    int get = progressBar.getSecondaryProgress();
                    progressBar.incrementSecondaryProgressBy(precent - get);
                }
                break;

                case 4:{
                    float stp = Float.valueOf((String) msg.obj);
                    int precent = (int) (stp / duration * 100);
                    int get = progressBar.getProgress();
                    progressBar.incrementProgressBy(precent - get);
                }
                break;

                default:
                    break;
            }
        };
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);

        imageView = (ImageView)findViewById(R.id.b_pic);
        editText = (EditText)findViewById(R.id.edit_text);
        textView = (TextView)findViewById(R.id.base_info);
        progressBar = (ProgressBar)findViewById(R.id.progressBarHorizontal);

        player = CNPlayer.getAudioPlayerInstance(this);
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
        player.setPrepared(editText.getText().toString());
    }

    public void audio_start(View view) {
        player.start(this);
    }

    public void audio_stop(View view) {
        player.stop();
    }

    public void audio_pause(View view) {
        player.pause();
    }

    @Override
    public void onPrepared(String s) {
        CNTrace.d("onPrepared : " + s);
        CNTrace.d("player statue : " + player.getStatus());
    }

    @Override
    public void onError(String s, int errorCode) {
        CNTrace.d(s + ", errorCode : " + errorCode);
    }

    @Override
    public void onMetaData(String key, String value) {
        Message msg = mHandler.obtainMessage();
        msg.what = 2;
        msg.obj = key + " : " + value;
        mHandler.sendMessage(msg);
    }

    @Override
    public void onBaseInfo(String key, String value) {
        if(key.contentEquals("duration")){
            duration = Float.valueOf(value);
            CNTrace.d("get duration : " + duration);
        }
        Message msg = mHandler.obtainMessage();
        msg.what = 2;
        msg.obj = key + " : " + value;
        mHandler.sendMessage(msg);
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
        CNTrace.d("update : " + update);
        Message msg = mHandler.obtainMessage();
        msg.what = 3;
        msg.obj = update;
        mHandler.sendMessage(msg);
    }

    @Override
    public void onPlayProgressing(String update) {
        CNTrace.d(update);
        Message msg = mHandler.obtainMessage();
        msg.what = 4;
        msg.obj = update;
        mHandler.sendMessage(msg);
    }
}
