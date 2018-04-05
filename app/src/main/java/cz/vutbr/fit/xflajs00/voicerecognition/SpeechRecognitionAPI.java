package cz.vutbr.fit.xflajs00.voicerecognition;

import java.util.ArrayList;
import java.util.List;

public class SpeechRecognitionAPI {
    private List<ISpeechRecognitionAPICallback> listeners = new ArrayList<>();

    private void notifyVADChanged(){
        for(ISpeechRecognitionAPICallback listener : listeners) {
            listener.onVADChanged();
        }
    }

    private void notifyOnSequenceRecognized(){
        for(ISpeechRecognitionAPICallback listener : listeners) {
            listener.onSequenceRecognized();
        }
    }

    private void notifyOnRecognitionDone(){
        for(ISpeechRecognitionAPICallback listener : listeners) {
            listener.onRecognitionDone();
        }
    }

    public void registerCallbacks(ISpeechRecognitionAPICallback listener){
        listeners.add(listener);
    }

    public interface ISpeechRecognitionAPICallback {
        void onVADChanged();

        void onSequenceRecognized();

        void onRecognitionDone();
    }
}
