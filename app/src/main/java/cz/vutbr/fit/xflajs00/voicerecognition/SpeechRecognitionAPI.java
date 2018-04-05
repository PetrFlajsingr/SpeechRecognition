package cz.vutbr.fit.xflajs00.voicerecognition;

import java.util.ArrayList;
import java.util.List;

public class SpeechRecognitionAPI {
    // NATIVE METHODS
    public native void setCacheDir(String cacheDir);
    public native void createEngine();
    public native boolean createAudioRecorder();
    public native void startRecording();
    public native void stopRecording();
    public native void shutdown();

    static{
        System.loadLibrary("speech_recognition");
    }
    //\ NATIVE METHODS



    // LISTENERS
    private List<ISpeechRecognitionAPICallback> listeners = new ArrayList<>();

    private VAD_Activity activity = VAD_Activity.INACTIVE;

    private void notifyVADChanged(){
        for(ISpeechRecognitionAPICallback listener : listeners) {
            listener.onVADChanged(activity);
        }
    }

    private void notifyOnSequenceRecognized(String sequence){
        for(ISpeechRecognitionAPICallback listener : listeners) {
            listener.onSequenceRecognized(sequence);
        }
    }

    private void notifyOnRecognitionDone(){
        for(ISpeechRecognitionAPICallback listener : listeners) {
            listener.onRecognitionDone();
        }
    }

    public void addListener(ISpeechRecognitionAPICallback listener){
        listeners.add(listener);
    }

    public enum VAD_Activity{
        ACTIVE,
        INACTIVE
    }

    public interface ISpeechRecognitionAPICallback {
        void onVADChanged(VAD_Activity activity);

        void onSequenceRecognized(String sequence);

        void onRecognitionDone();
    }
    //\ LISTENERS
}
