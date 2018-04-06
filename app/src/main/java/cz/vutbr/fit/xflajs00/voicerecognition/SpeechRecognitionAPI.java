package cz.vutbr.fit.xflajs00.voicerecognition;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * Class providing an interface to use a native library for speech recognition.
 * Uses observer pattern to notify listeners about certain events.
 * Usage:
 * SpeechRecognitionAPI speechRecognitionAPI = SpeechRecognitionAPI(<activity>.getCacheDir.toString());
 * speechRecognitionAPI.addListener(<class implementing SpeechRecognitionAPI.ISpeechRecognitionAPICallback interface>);
 *
 * For recognition from microphone:
 * --- IN ANOTHER THREAD: ---
 * speechRecognitionAPI.startRecording();
 * --------------------------
 * speechRecognitionAPI.stopRecording();
 *
 * --- When no longer needed: ---
 * speechRecognitionAPI.shutdown();
 *
 * For recognition of an audio file
 * speechRecognitionAPI.recognizeWAV(<path to WAV file>);
 *
 */
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

    private native void registerCallbacksNative();

    static{
        System.loadLibrary("speech_recognition");
    }
    //\ NATIVE METHODS

    private boolean recorderCreated = false;

    private boolean recording = false;
    public boolean isRecording() {
        return recording;
    }

    /**
     * Requires cache dir path for RenderScript
     * @param cacheDir cache dir path
     */
    public SpeechRecognitionAPI(String cacheDir) {
        setCacheDirNative(cacheDir);
        createEngineNative();

        registerCallbacksNative();
    }

    /**
     * Starts recording in native code and starts the recognition at the same time.
     * @throws Exception Thrown when audio recorder can not be created
     */
    public void startRecording() throws Exception {
        if(!recorderCreated){
            if(!createAudioRecorderNative()){
                throw new Exception("Failed to create audio recorder");
            }
            recorderCreated = true;
        }

        startRecordingNative();
        recording = true;
    }

    /**
     * Stops microphone recording
     */
    public void stopRecording(){
        stopRecordingNative();
        recording = false;
    }

    /**
     * Destroys allocated memory for audio recording
     */
    public void shutdown(){
        shutdownNative();
    }

    /**
     * Recognizes speech from a given audio file.
     * @param file WAV audio file
     */
    public void recognizeWAV(File file){
        // TODO implement
        throw new UnsupportedOperationException("The method has not been implemented");
    }

    // CALLBACKS FROM NDK
    private void VADChanged(boolean activity){
        if(activity){
            notifyVADChanged(VAD_ACTIVITY.ACTIVE);
        } else {
            notifyVADChanged(VAD_ACTIVITY.INACTIVE);
        }
    }

    private void sequenceRecognized(String sequence){
        notifySequenceRecognized(sequence);
    }

    private void recognitionDone(){
        notifyRecognitionDone();
    }

    //\ CALLBACKS FROM NDK


    // LISTENERS
    private List<ISpeechRecognitionAPICallback> listeners = new ArrayList<>();

    private void notifyVADChanged(VAD_ACTIVITY activity){
        for(ISpeechRecognitionAPICallback listener : listeners) {
            listener.onVADChanged(activity);
        }
    }

    private void notifySequenceRecognized(String sequence){
        for(ISpeechRecognitionAPICallback listener : listeners) {
            listener.onSequenceRecognized(sequence);
        }
    }

    private void notifyRecognitionDone(){
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

    /**
     * Interface for implementation of listener functions.
     */
    public interface ISpeechRecognitionAPICallback {
        void onVADChanged(VAD_ACTIVITY activity);

        void onSequenceRecognized(String sequence);

        void onRecognitionDone();
    }
    //\ LISTENERS
}
