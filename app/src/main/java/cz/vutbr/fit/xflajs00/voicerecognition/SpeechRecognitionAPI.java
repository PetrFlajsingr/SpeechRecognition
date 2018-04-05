package cz.vutbr.fit.xflajs00.voicerecognition;

import java.util.ArrayList;
import java.util.List;

public class SpeechRecognitionAPI {
    // TEST

    private native void testNative();

    public void test(){
        testNative();
    }

    //\ TEST
    // NATIVE METHODS
    private native void setCacheDirNative(String cacheDir);
    private native void createEngineNative();
    private native boolean createAudioRecorderNative();
    private native void startRecordingNative();
    private native void stopRecordingNative();
    private native void shutdownNative();

    static{
        System.loadLibrary("speech_recognition");
    }
    //\ NATIVE METHODS

    private boolean recorderCreated = false;

    private boolean recording = false;
    public boolean isRecording() {
        return recording;
    }

    public SpeechRecognitionAPI(String cacheDir) {
        setCacheDirNative(cacheDir);
        createEngineNative();
    }

    public void startRecording(){
        if(!recorderCreated){
            createAudioRecorderNative();
            recorderCreated = true;
        }

        startRecordingNative();
        recording = true;
    }

    public void stopRecording(){
        stopRecordingNative();
        recording = false;
    }

    public void shutdown(){
        shutdownNative();
    }

    // CALLBACKS FROM NDK


    //\ CALLBACKS FROM NDK


    // LISTENERS
    private List<ISpeechRecognitionAPICallback> listeners = new ArrayList<>();

    private VAD_ACTIVITY activity = VAD_ACTIVITY.INACTIVE;

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

    public enum VAD_ACTIVITY {
        ACTIVE,
        INACTIVE
    }

    public interface ISpeechRecognitionAPICallback {
        void onVADChanged(VAD_ACTIVITY activity);

        void onSequenceRecognized(String sequence);

        void onRecognitionDone();
    }
    //\ LISTENERS
}
