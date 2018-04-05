package cz.vutbr.fit.xflajs00.voicerecognition;

public class SpeechRecognitionAPI {

    public interface ISpeechRecognitionAPICallback {
        void onVADChanged();

        void onSequenceRecognized();

        void onRecognitionDone();
    }

}
