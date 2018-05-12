//
// Created by Petr Flajsingr on 07/04/2018.
//

#ifndef VOICERECOGNITION_QUEUEDATA_H
#define VOICERECOGNITION_QUEUEDATA_H
/**
 * Data types for passing between worker threads
 */

/**
 * Type of data - control
 */
enum DATA_TYPE{
    SEQUENCE_DATA,
    SEQUENCE_INACTIVE,
    TERMINATE
};

/**
 * Audio data from recorder
 */
typedef struct s_AudioData{
    DATA_TYPE type;
    short* data;
}Q_AudioData;

/**
 * Mel data from MelBankThread
 */
typedef struct s_MelData{
    DATA_TYPE type;
    float* data;
}Q_MelData;

/**
 * NN output from NNThread
 */
typedef struct s_NNData{
    DATA_TYPE type;
    float* data;
}Q_NNData;
#endif //VOICERECOGNITION_QUEUEDATA_H
