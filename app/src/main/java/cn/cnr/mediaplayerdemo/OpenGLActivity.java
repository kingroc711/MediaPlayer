package cn.cnr.mediaplayerdemo;

import android.app.ActivityManager;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Toast;
import cn.cnr.mediaplayer.R;
import cn.cnr.player.CNTrace;

public class OpenGLActivity extends AppCompatActivity {
    private GLSurfaceView glSurfaceView = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        final ActivityManager activityManager=(ActivityManager)getSystemService(ACTIVITY_SERVICE);
        final ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
        boolean supportsEs2 = configurationInfo.reqGlEsVersion >= 0x2000;

        CNTrace.d("version : " + configurationInfo.reqGlEsVersion + ", support : " + supportsEs2);
        if(supportsEs2){
            glSurfaceView = new GLSurfaceView(this);
            glSurfaceView.setRenderer(new GLRenderer());
            setContentView(glSurfaceView);
        }else{
            setContentView(R.layout.activity_open_gl);
            Toast.makeText(this, "Device not support OpenGL SE 2.0 !", Toast.LENGTH_LONG).show();
        }
    }

    @Override
    protected void onPause(){
        super.onPause();
        if(glSurfaceView != null)
            glSurfaceView.onPause();

    }

    @Override
    protected void onResume(){
        super.onResume();
        if(glSurfaceView != null)
            glSurfaceView.onResume();
    }
}
