package cz.vutbr.fit.xflajs00.voicerecognition;

import android.Manifest;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Build;
import android.os.Debug;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Main activity of this demo application
 *
 * @author Petr Flajšingr
 */
public class MainActivity extends AppCompatActivity implements SpeechRecognitionAPI.ISpeechRecognitionAPICallback, SpeechRecognitionAPI.ISpeechRecognitionAPIDebugCallbacks{
    private boolean recording = false;

    private SpeechRecognitionAPI speechAPI;

    private TextView resultTextView;

    private TextView VADTextView;

    private ScrollView scrollView;

    private TextView melThreadTextView;
    private TextView nnThreadTextView;
    private TextView decoderThreadTextView;
    private TextView memoryUsageTextView;

    private Button recordingButton;
    private Button wavButton;

    final private Timer timer = new Timer();

    /**
     * Requests permissions on start.
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        int permissions_code = 42;
        String[] permissions = {Manifest.permission.RECORD_AUDIO, Manifest.permission.MODIFY_AUDIO_SETTINGS, Manifest.permission.WRITE_EXTERNAL_STORAGE};

        ActivityCompat.requestPermissions(this, permissions, permissions_code);

        try {
            speechAPI = new SpeechRecognitionAPI(this.getCacheDir().toString());
        } catch (Exception e) {
            AlertDialog.Builder alertBuilder = new AlertDialog.Builder(this);

            alertBuilder.setMessage(e.getMessage() + "\n" + getResources().getString(R.string.will_terminate))
                    .setTitle(R.string.criticalError)
                    .setCancelable(false)
                    .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            dialogInterface.dismiss();

                            System.exit(-99);
                        }
                    });

            AlertDialog dialog = alertBuilder.create();

            dialog.show();

            e.printStackTrace();

            return;
        }
        speechAPI.addListener(this);

        speechAPI.addDebugListener(this);

        resultTextView = (TextView) findViewById(R.id.textViewRecognitionResult);
        VADTextView = (TextView) findViewById(R.id.VADTextView);

        scrollView = (ScrollView)findViewById(R.id.scrollView2);

        scrollView.post(new Runnable() {
            @Override
            public void run() {
                scrollView.fullScroll(ScrollView.FOCUS_DOWN);
            }
        });

        melThreadTextView = (TextView) findViewById(R.id.melThreadTextView);
        nnThreadTextView = (TextView) findViewById(R.id.nnThreadTextView);
        decoderThreadTextView = (TextView) findViewById(R.id.decoderThreadTextView);
        memoryUsageTextView = (TextView) findViewById(R.id.memoryUsageTextView);

        recordingButton = (Button) findViewById(R.id.buttonRecognitionControl);
        wavButton = (Button) findViewById(R.id.button);

        timer.schedule(new memoryTask(), 0, 2000);
    }

    @Override
    protected void onResume(){
        super.onResume();
    }

    /** Called when the activity is about to be destroyed. */
    @Override
    protected void onDestroy()
    {
        speechAPI.shutdown();
        super.onDestroy();
    }
    // UI control with a button
    public void recordingControl(View view){
        if(!recording){
            wavButton.setEnabled(false);
            recording = true;
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
            recording = false;
            speechAPI.stopRecording();
        }
    }


    public void wavButtonOnClick(View view){
        Toast.makeText(getApplicationContext(), "Running wav recognition", Toast.LENGTH_LONG).show();
        wavButton.setEnabled(false);
        recordingButton.setEnabled(false);
        Thread thread = new Thread() {
            @Override
            public void run() {
                final String recognized = speechAPI.recognizeWAV("/sdcard/speechrecognition/test.wav");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        resultTextView.setText(recognized);
                    }
                });
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
                String origText = resultTextView.getText().toString();
                if(origText.length() > 2)
                    origText = origText.substring(0, origText.length() - 1);
                resultTextView.setText(String.format("%s\n%s\n", origText, finalSequence));
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
                wavButton.setEnabled(true);
                recordingButton.setEnabled(true);
            }
        });
    }

    @Override
    public void onMelDone(final int percentage) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                melThreadTextView.setText(String.format("%s%%", Integer.toString(percentage)));
            }
        });
    }

    @Override
    public void onNNDone(final int percentage) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                nnThreadTextView.setText(String.format("%s%%", Integer.toString(percentage)));
            }
        });
    }

    @Override
    public void onDecoderDone(final int percentage) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                decoderThreadTextView.setText(String.format("%s%%", Integer.toString(percentage)));
            }
        });
    }

    private String getMemoryUsage(){
        Debug.MemoryInfo memInfo = new Debug.MemoryInfo();

        Debug.getMemoryInfo(memInfo);

        long res = memInfo.getTotalPrivateDirty();
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT){
            res += memInfo.getTotalPrivateClean();
        }

        return Long.toString(res/1000) + " MB";
    }

    class memoryTask extends TimerTask{
        @Override
        public void run() {
            final String memInfo = getMemoryUsage();
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    memoryUsageTextView.setText(memInfo);
                }
            });
        }
    }
}
