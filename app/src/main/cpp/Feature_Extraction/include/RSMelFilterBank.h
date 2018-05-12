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

    /**
     * @brief Class handling mel bank filters
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class RSMelFilterBank : public RSBase {
    private:
        ScriptC_RSmelfilterbank *melRSinstance;

        /**
         * Prepares allocations for Renderscript.
         */
        void prepareAllocations();

        // Allocations
        sp<Allocation> fftFrameAllocation;
        sp<Allocation> melIterationAllocation;
        sp<Allocation> melBankAllocation;

        float meanForNormalisation[MEL_BANK_FRAME_LENGTH] = {0};

        unsigned int elementCount = 0;
    public:
        /**
         * Requires cache directory of the application for use in RenderScript
         * @param cacheDir cache directory of the application
         */
        RSMelFilterBank(const char *cacheDir);

        /**
         * Calculates mel bank filters for given data.
         */
        float *calculateMelBank(kiss_fft_cpx *fftData);

        /**
         * Normalises provided data
         * @param data mel bank output
         */
        void normalise(float *data);
    };
}


#endif //VOICERECOGNITION_RSMELFILTERBANK_H
