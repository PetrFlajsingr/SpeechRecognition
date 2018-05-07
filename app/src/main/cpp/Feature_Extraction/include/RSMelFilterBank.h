//
// Created by Petr Flajsingr on 15/12/2017.
//

#ifndef VOICERECOGNITION_RSMELFILTERBANK_H
#define VOICERECOGNITION_RSMELFILTERBANK_H

#include <RenderScript.h>
#include <FeatureMatrix.h>
#include <kiss_fft.h>
#include <constants.h>
#include "ScriptC_RSmelfilterbank.h"
#include "RSBase.h"

using namespace android::RSC;

namespace SpeechRecognition::Feature_Extraction {
    using namespace Utility;
    class RSMelFilterBank : public RSBase {
    private:
        ScriptC_RSmelfilterbank *melRSinstance;

        void prepareAllocations();

        // Allocations
        sp<Allocation> fftFrameAllocation;
        sp<Allocation> melIterationAllocation;
        sp<Allocation> melBankAllocation;

        float meanForNormalisation[MEL_BANK_FRAME_LENGTH] = {0};

        unsigned int elementCount = 0;
    public:
        RSMelFilterBank(const char *cacheDir);

        float *calculateMelBank(kiss_fft_cpx *fftData);

        void normalise(float *data);
    };
}


#endif //VOICERECOGNITION_RSMELFILTERBANK_H
