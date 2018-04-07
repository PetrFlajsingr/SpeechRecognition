//
// Created by Petr Flajsingr on 07/04/2018.
//

#ifndef VOICERECOGNITION_QUEUEDATA_H
#define VOICERECOGNITION_QUEUEDATA_H
enum DATA_TYPE{
    SEQUENCE_DATA,
    SEQUENCE_INACTIVE
};
typedef struct s_AudioData{
    short* data;
}Q_AudioData;

typedef struct s_MelData{
    DATA_TYPE type;
    float* data;
}Q_MelData;

typedef struct s_NNData{
    DATA_TYPE type;
    float* data;
}Q_NNData;
#endif //VOICERECOGNITION_QUEUEDATA_H
