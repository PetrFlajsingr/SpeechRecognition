//
// Created by Petr Flajsingr on 15/12/2017.
//

#ifndef VOICERECOGNITION_RSMELFILTERBANK_H
#define VOICERECOGNITION_RSMELFILTERBANK_H

#include <RenderScript.h>
#include <FeatureMatrix.h>
#include <kiss_fft.h>
#include "ScriptC_RSmelfilterbank.h"

using namespace android::RSC;

class RSMelFilterBank {
private:
    sp<RS> renderScriptObject;

    ScriptC_RSmelfilterbank* melRSinstance;

    void prepareAllocations();

    // Allocations
    sp<Allocation> fftFrameAllocation;
    sp<Allocation> melIterationAllocation;
    sp<Allocation> melBankAllocation;
public:
    RSMelFilterBank(const char* cacheDir);

    float* calculateMelBank(kiss_fft_cpx* fftData);

    void substractMean(FeatureMatrix* featuresMatrix);
};


#endif //VOICERECOGNITION_RSMELFILTERBANK_H
