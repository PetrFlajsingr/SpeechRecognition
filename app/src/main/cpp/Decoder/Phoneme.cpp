//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <Phoneme.h>

/**
 * Returns enum value of a phoneme given string form.
 * @param phoneme string form of a phoneme
 */
SpeechRecognition::Decoder::PHONEME SpeechRecognition::Decoder::Phoneme::stringToPhoneme(std::string phoneme) {
    if(phoneme == "AA") return PHONEME::AA;
    if(phoneme == "AE") return PHONEME::AE;
    if(phoneme == "AH") return PHONEME::AH;
    if(phoneme == "AO") return PHONEME::AO;
    if(phoneme == "AW") return PHONEME::AW;
    if(phoneme == "AY") return PHONEME::AY;
    if(phoneme == "B") return PHONEME::B;
    if(phoneme == "BRH") return PHONEME::BRH;
    if(phoneme == "CGH") return PHONEME::CGH;
    if(phoneme == "CH") return PHONEME::CH;
    if(phoneme == "D") return PHONEME::D;
    if(phoneme == "DH") return PHONEME::DH;
    if(phoneme == "EH") return PHONEME::EH;
    if(phoneme == "ER") return PHONEME::ER;
    if(phoneme == "EY") return PHONEME::EY;
    if(phoneme == "F") return PHONEME::F;
    if(phoneme == "G") return PHONEME::G;
    if(phoneme == "HH") return PHONEME::HH;
    if(phoneme == "IH") return PHONEME::IH;
    if(phoneme == "IY") return PHONEME::IY;
    if(phoneme == "JH") return PHONEME::JH;
    if(phoneme == "K") return PHONEME::K;
    if(phoneme == "L") return PHONEME::L;
    if(phoneme == "M") return PHONEME::M;
    if(phoneme == "N") return PHONEME::N;
    if(phoneme == "NG") return PHONEME::NG;
    if(phoneme == "NSN") return PHONEME::NSN;
    if(phoneme == "OW") return PHONEME::OW;
    if(phoneme == "OY") return PHONEME::OY;
    if(phoneme == "P") return PHONEME::P;
    if(phoneme == "R") return PHONEME::R;
    if(phoneme == "S") return PHONEME::S;
    if(phoneme == "SH") return PHONEME::SH;
    if(phoneme == "SIL") return PHONEME::SIL;
    if(phoneme == "SMK") return PHONEME::SMK;
    if(phoneme == "T") return PHONEME::T;
    if(phoneme == "TH") return PHONEME::TH;
    if(phoneme == "UH") return PHONEME::UH;
    if(phoneme == "UHH") return PHONEME::UHH;
    if(phoneme == "UM") return PHONEME::UM;
    if(phoneme == "UW") return PHONEME::UW;
    if(phoneme == "V") return PHONEME::V;
    if(phoneme == "W") return PHONEME::W;
    if(phoneme == "Y") return PHONEME::Y;
    if(phoneme == "Z") return PHONEME::Z;
    if(phoneme == "ZH") return PHONEME::ZH;

    return PHONEME::NONE;
}
