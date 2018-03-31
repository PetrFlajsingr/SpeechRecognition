//
// Created by Petr Flajsingr on 20/03/2018.
//

#ifndef VOICERECOGNITION_RSBASE_H
#define VOICERECOGNITION_RSBASE_H

#include <RenderScript.h>

class RSBase {
protected:
    sp<RS> renderscriptObject;

    android::RSC::ScriptC* scriptInstance;

    virtual void prepareAllocations() = 0;

public:
    virtual ~RSBase();
};


#endif //VOICERECOGNITION_RSBASE_H
