//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <Phoneme.h>

PHONEME_ENUM Phoneme::stringToPhoneme(std::string phoneme) {
    if(phoneme == "AA") return PHONEME_ENUM::AA;
    if(phoneme == "AE") return PHONEME_ENUM::AE;
    if(phoneme == "AH") return PHONEME_ENUM::AH;
    if(phoneme == "AO") return PHONEME_ENUM::AO;
    if(phoneme == "AW") return PHONEME_ENUM::AW;
    if(phoneme == "AY") return PHONEME_ENUM::AY;
    if(phoneme == "B") return PHONEME_ENUM::B;
    if(phoneme == "BRH") return PHONEME_ENUM::BRH;
    if(phoneme == "CGH") return PHONEME_ENUM::CGH;
    if(phoneme == "CH") return PHONEME_ENUM::CH;
    if(phoneme == "D") return PHONEME_ENUM::D;
    if(phoneme == "DH") return PHONEME_ENUM::DH;
    if(phoneme == "EH") return PHONEME_ENUM::EH;
    if(phoneme == "ER") return PHONEME_ENUM::ER;
    if(phoneme == "EY") return PHONEME_ENUM::EY;
    if(phoneme == "F") return PHONEME_ENUM::F;
    if(phoneme == "G") return PHONEME_ENUM::G;
    if(phoneme == "HH") return PHONEME_ENUM::HH;
    if(phoneme == "IH") return PHONEME_ENUM::IH;
    if(phoneme == "IY") return PHONEME_ENUM::IY;
    if(phoneme == "JH") return PHONEME_ENUM::JH;
    if(phoneme == "K") return PHONEME_ENUM::K;
    if(phoneme == "L") return PHONEME_ENUM::L;
    if(phoneme == "M") return PHONEME_ENUM::M;
    if(phoneme == "N") return PHONEME_ENUM::N;
    if(phoneme == "NG") return PHONEME_ENUM::NG;
    if(phoneme == "NSN") return PHONEME_ENUM::NSN;
    if(phoneme == "OW") return PHONEME_ENUM::OW;
    if(phoneme == "OY") return PHONEME_ENUM::OY;
    if(phoneme == "P") return PHONEME_ENUM::P;
    if(phoneme == "R") return PHONEME_ENUM::R;
    if(phoneme == "S") return PHONEME_ENUM::S;
    if(phoneme == "SH") return PHONEME_ENUM::SH;
    if(phoneme == "SIL") return PHONEME_ENUM::SIL;
    if(phoneme == "SMK") return PHONEME_ENUM::SMK;
    if(phoneme == "T") return PHONEME_ENUM::T;
    if(phoneme == "TH") return PHONEME_ENUM::TH;
    if(phoneme == "UH") return PHONEME_ENUM::UH;
    if(phoneme == "UHH") return PHONEME_ENUM::UHH;
    if(phoneme == "UM") return PHONEME_ENUM::UM;
    if(phoneme == "UW") return PHONEME_ENUM::UW;
    if(phoneme == "V") return PHONEME_ENUM::V;
    if(phoneme == "W") return PHONEME_ENUM::W;
    if(phoneme == "Y") return PHONEME_ENUM::Y;
    if(phoneme == "Z") return PHONEME_ENUM::Z;
    if(phoneme == "ZH") return PHONEME_ENUM::ZH;

    return PHONEME_ENUM::NONE;
}
