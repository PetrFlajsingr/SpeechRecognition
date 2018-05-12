//
// Created by Petr Flajsingr on 01/05/2018.
//

#ifndef VOICERECOGNITION_RSBASE_H
#define VOICERECOGNITION_RSBASE_H

#include <rsCppStructs.h>

using namespace android::RSC;

namespace SpeechRecognition::Feature_Extraction {
    /**
     * @brief Base class for Renderscript using classes
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class RSBase {
    protected:
        sp <RS> renderScriptObject;
    };
}


#endif //VOICERECOGNITION_RSBASE_H
