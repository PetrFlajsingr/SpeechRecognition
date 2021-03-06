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
    private String cacheDir;

    // NATIVE METHODS
    private native String setCacheDirNative(String cacheDir);
    private native boolean startRecordingNative();
    private native void stopRecordingNative();
    private native void shutdownNative();

    private native boolean isRecordingNative();

    private native void registerCallbacksNative();

    private native String recognizeWAVNative(String path);

    static{
        System.loadLibrary("speech_recognition");
    }
    //\ NATIVE METHODS


    public boolean isRecording() {
        return isRecordingNative();
    }

    /**
     * Requires cache dir path for RenderScript
     * @param cacheDir cache dir path
     * @throws Exception when file check fails
     */
    public SpeechRecognitionAPI(String cacheDir) throws Exception {
        this.cacheDir = cacheDir;

        String errorMsg = setCacheDirNative(cacheDir);

        if(errorMsg.length() != 0)
            throw new Exception(errorMsg);

        registerCallbacksNative();
    }

    /**
     * Starts recording in native code and starts the recognition at the same time.
     * @throws Exception Thrown when audio recorder can not be created
     */
    public void startRecording() throws Exception {
        if(!startRecordingNative())
            throw new Exception("Failed to create audio recorder");
    }

    /**
     * Stops microphone recording
     */
    public void stopRecording(){
        stopRecordingNative();
    }

    /**
     * Destroys allocated memory for audio recording
     */
    public void shutdown(){
        shutdownNative();
    }

    /**
     * Recognizes speech from a given audio file.
     * @param filePath path to WAV audio file
     */
    public String recognizeWAV(String filePath){
        return recognizeWAVNative(filePath);
    }

    // CALLBACKS FROM NDK
    /**
     * These functions are called from NDK via JNI.
     */
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
    final private List<ISpeechRecognitionAPICallback> listeners = new ArrayList<>();

    private void notifyVADChanged(final VAD_ACTIVITY activity){
        new Thread(new Runnable() {
            @Override
            public void run() {
                for(ISpeechRecognitionAPICallback listener : listeners) {
                    listener.onVADChanged(activity);
                }
            }
        }).run();
    }

    private void notifySequenceRecognized(final String sequence){
        new Thread(new Runnable() {
            @Override
            public void run() {
                for(ISpeechRecognitionAPICallback listener : listeners) {
                    listener.onSequenceRecognized(sequence);
                }
            }
        }).run();
    }

    private void notifyRecognitionDone(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                for(ISpeechRecognitionAPICallback listener : listeners) {
                    listener.onRecognitionDone();
                }
            }
        }).run();
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

    final private List<ISpeechRecognitionAPIDebugCallbacks> debugListeners = new ArrayList<>();

    public void addDebugListener(ISpeechRecognitionAPIDebugCallbacks listener){
        debugListeners.add(listener);
    }

    private void melDone(int percentage){
        notifyMelDone(percentage);
    }

    private void notifyMelDone(final int percentage){
        new Thread(new Runnable() {
            @Override
            public void run() {
                for(ISpeechRecognitionAPIDebugCallbacks listener : debugListeners) {
                    listener.onMelDone(percentage);
                }
            }
        }).run();
    }

    private void nnDone(int percentage){
        notifyNNDone(percentage);
    }

    private void notifyNNDone(final int percentage){
        new Thread(new Runnable() {
            @Override
            public void run() {
                for(ISpeechRecognitionAPIDebugCallbacks listener : debugListeners) {
                    listener.onNNDone(percentage);
                }
            }
        }).run();
    }

    private void decoderDone(int percentage){
        notifyDecoderDone(percentage);
    }

    private void notifyDecoderDone(final int percentage){
        new Thread(new Runnable() {
            @Override
            public void run() {
                for(ISpeechRecognitionAPIDebugCallbacks listener : debugListeners) {
                    listener.onDecoderDone(percentage);
                }
            }
        }).run();
    }

    public interface ISpeechRecognitionAPIDebugCallbacks{
        void onMelDone(final int percentage);
        void onNNDone(final int percentage);
        void onDecoderDone(final int percentage);
    }
}
