package cz.vutbr.fit.xflajs00.voicerecognition;

import android.Manifest;
import android.graphics.Color;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements SpeechRecognitionAPI.ISpeechRecognitionAPICallback{
    private boolean recording = false;
    private boolean created = false;

    private SpeechRecognitionAPI speechAPI;

    private TextView resultTextView;

    private TextView VADTextView;

    private ScrollView scrollView;

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

        speechAPI = new SpeechRecognitionAPI(this.getCacheDir().toString());
        speechAPI.addListener(this);

        resultTextView = (TextView) findViewById(R.id.textViewRecognitionResult);
        VADTextView = (TextView) findViewById(R.id.VADTextView);

        scrollView = (ScrollView)findViewById(R.id.scrollView2);

        scrollView.post(new Runnable() {
            @Override
            public void run() {
                scrollView.fullScroll(ScrollView.FOCUS_DOWN);
            }
        });
    }

    @Override
    protected void onResume(){
        super.onResume();
    }

    /** Called when the activity is about to be destroyed. */
    @Override
    protected void onDestroy()
    {
        //shutdown();
        speechAPI.shutdown();
        super.onDestroy();
    }
    // UI control with a button
    public void recordingControl(View view){
        recording = !recording;
        if(recording){
            VADTextView.setVisibility(View.VISIBLE);
            resultTextView.setText("");
            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        speechAPI.startRecording();
                    } catch (Exception e) {
                        e.printStackTrace();
                        recording = false;
                    }
                }
            }).run();
        }else{
            speechAPI.stopRecording();
        }
    }


    public void createFrames(View view){
        Thread thread = new Thread() {
            @Override
            public void run() {
                speechAPI.test();
            }
        };
        thread.start();

    }

    @Override
    public void onVADChanged(SpeechRecognitionAPI.VAD_ACTIVITY activity) {
        final String text;
        final int textColor;

        if(activity == SpeechRecognitionAPI.VAD_ACTIVITY.ACTIVE){
            text = getString(R.string.active);
            textColor = Color.GREEN;
        } else {
            text = getString(R.string.inactive);
            textColor = Color.RED;
        }
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                VADTextView.setText(text);
                VADTextView.setTextColor(textColor);
            }
        });
    }

    @Override
    public void onSequenceRecognized(String sequence) {
        final String finalSequence = sequence;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                resultTextView.setText(String.format("%s\n%s", resultTextView.getText(), finalSequence));
                scrollView.fullScroll(ScrollView.FOCUS_DOWN);
            }
        });
    }

    @Override
    public void onRecognitionDone() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), "RECOGNITION DONE", Toast.LENGTH_LONG).show();
                VADTextView.setVisibility(View.INVISIBLE);
            }
        });
    }
}
