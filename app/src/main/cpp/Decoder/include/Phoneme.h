//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_PHONEME_H
#define VOICERECOGNITION_PHONEME_H

#include <string>

// phonemes coresponding to their output index in neural network
enum PHONEME_ENUM{
    AA  =   0,
    AE  =   1,
    AH  =   2,
    AO  =   3,
    AW  =   4,
    AY  =   5,
    B   =   6,
    BRH =   7,
    CGH =   8,
    CH  =   9,
    D   =   10,
    DH  =   11,
    EH  =   12,
    ER  =   13,
    EY  =   14,
    F   =   15,
    G   =   16,
    HH  =   17,
    IH  =   18,
    IY  =   19,
    JH  =   20,
    K   =   21,
    L   =   22,
    M   =   23,
    N   =   24,
    NG  =   25,
    NSN =   26,
    OW  =   27,
    OY  =   28,
    P   =   29,
    R   =   30,
    S   =   31,
    SH  =   32,
    SIL =   33,
    SMK =   34,
    T   =   35,
    TH  =   36,
    UH  =   37,
    UHH =   38,
    UM  =   39,
    UW  =   40,
    V   =   41,
    W   =   42,
    Y   =   43,
    Z   =   44,
    ZH  =   45
};

class Phoneme {
public:
    static PHONEME_ENUM stringToPhoneme(std::string phoneme);

};


#endif //VOICERECOGNITION_PHONEME_H
