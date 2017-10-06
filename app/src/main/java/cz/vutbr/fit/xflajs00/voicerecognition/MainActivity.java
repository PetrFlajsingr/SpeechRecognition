package cz.vutbr.fit.xflajs00.voicerecognition;

import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    private boolean recording = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    // Ovládání UI

    public void recordingControl(View view){
        recording = !recording;
       // Button button = (Button) findViewById(R.id.buttonRecognitionControl);
        if(recording){
            //button.setText(R.string.stopRecording);
        }else{
          //  button.setText(R.string.startRecording);
            ProgressBar bar = (ProgressBar) findViewById(R.id.progressBarRecognizing);
            bar.setIndeterminate(true);
            bar.setVisibility(View.VISIBLE);

            final Handler handler = new Handler();
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    ProgressBar bar = (ProgressBar) findViewById(R.id.progressBarRecognizing);
                    bar.setVisibility(View.INVISIBLE);
                }}, 5000);
        }

    }
}
