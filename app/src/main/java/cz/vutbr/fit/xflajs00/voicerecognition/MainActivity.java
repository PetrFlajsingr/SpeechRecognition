package cz.vutbr.fit.xflajs00.voicerecognition;

import android.Manifest;
import android.graphics.Color;
import android.os.Handler;
import android.support.annotation.Keep;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import org.w3c.dom.Text;

public class MainActivity extends AppCompatActivity implements SpeechRecognitionAPI.ISpeechRecognitionAPICallback{
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

        //setCacheDir(this.getCacheDir().toString());
        //createEngine();
    }

    @Override
    protected void onResume(){
        super.onResume();
        //getJniString();
    }

    // please, let me live even though I used this dark programming technique
    public void messageMe(String text) {
        final String text2 = text;
        runOnUiThread(
                new Runnable() {
                    @Override
                    public void run() {
                        TextView view = (TextView)findViewById(R.id.textViewRecognitionResult);
                        view.setText(text2);
                    }
                }
        );
    }




    /** Called when the activity is about to be destroyed. */
    @Override
    protected void onDestroy()
    {
        //shutdown();
        super.onDestroy();
    }
    private Thread threadAudioRecorder;

    /*
    public static native void setCacheDir(String cacheDir);
    public static native void createEngine();
    public static native boolean createAudioRecorder();
    public static native void startRecording();
    public static native void stopRecording();
    public static native void shutdown();
    public static native void createFrames();
    public static native void threadTest();
    public native String getJniString();

    static{
        System.loadLibrary("raw_audio_recorder");
    }*/


    // UI control with a button
    public void recordingControl(View view){
        /*recording = !recording;
        if(recording){
            if (!created) {
                created = createAudioRecorder();
            }
            if(created){
                Thread thread = new Thread() {
                    @Override
                    public void run() {
                        threadTest();
                    }
                };
                thread.start();
            }else
                recording = false;
        }else{
            stopRecording();
        }*/
    }


    public void createFrames(View view){
        Thread thread = new Thread() {
            @Override
            public void run() {
                //createFrames();
            }
        };
        thread.start();

    }

    @Override
    public void onVADChanged(SpeechRecognitionAPI.VAD_Activity activity) {
        final String text;
        final int textColor;

        if(activity == SpeechRecognitionAPI.VAD_Activity.ACTIVE){
            text = getString(R.string.active);
            textColor = Color.GREEN;
        } else {
            text = getString(R.string.inactive);
            textColor = Color.RED;
        }
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                TextView view = (TextView) findViewById(R.id.VADTextView);
                view.setText(text);
                view.setTextColor(textColor);
            }
        });
    }

    @Override
    public void onSequenceRecognized(String sequence) {
        final String finalSequence = sequence;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                TextView view = (TextView) findViewById(R.id.textViewRecognitionResult);
                view.setText(String.format("%s%s", view.getText(), finalSequence));
            }
        });
    }

    @Override
    public void onRecognitionDone() {

    }
}
