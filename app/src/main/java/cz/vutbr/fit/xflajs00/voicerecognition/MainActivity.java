package cz.vutbr.fit.xflajs00.voicerecognition;

import android.Manifest;
import android.os.Handler;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    private boolean recording = false;
    private boolean created = false;

    /**
     * Requests permissions on start.
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, 1);
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.MODIFY_AUDIO_SETTINGS}, 1);
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);

        setCacheDir(this.getCacheDir().toString());
        createEngine();

    }

    /** Called when the activity is about to be destroyed. */
    @Override
    protected void onDestroy()
    {
        shutdown();
        super.onDestroy();
    }

    private Thread threadAudioRecorder;

    /** Native methods, implemented in jni folder */
    public static native void setCacheDir(String cacheDir);
    public static native void createEngine();
    public static native boolean createAudioRecorder();
    public static native void startRecording();
    public static native void stopRecording();
    public static native void shutdown();
    public static native void createFrames();
    public static native void threadTest();

    static{
        System.loadLibrary("raw_audio_recorder");
    }


    // UI control with a button
    public void recordingControl(View view){
        recording = !recording;
        if(recording){
            if (!created) {
                created = createAudioRecorder();
            }
            if(created)
                startRecording();
            else
                recording = false;
        }else{
            stopRecording();
        }
    }

    // UI control with a button
    public void threadTest(View view){
        createAudioRecorder();
        threadTest();
    }

    public void createFrames(View view){
        Thread thread = new Thread() {
            @Override
            public void run() {
                createFrames();
            }
        };
        thread.start();

    }
}
