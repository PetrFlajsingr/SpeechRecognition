//
// Created by Petr Flajsingr on 15/12/2017.
//

#include <constants.h>
#include "RSMelFilterBank.h"

/**
 * Requires cache directory of the application for use in RenderScript
 * @param cacheDir cache directory of the application
 */
RSMelFilterBank::RSMelFilterBank(const char* cacheDir) {
    this->renderScriptObject = new RS();
    this->renderScriptObject->init(cacheDir);
    this->melRSinstance = new ScriptC_RSmelfilterbank(this->renderScriptObject);

    this->prepareAllocations();
}

/**
 * Prepares allocations for Renderscript.
 */
void RSMelFilterBank::prepareAllocations() {
    Element::Builder* elBuilder = new Element::Builder(this->renderScriptObject);
    elBuilder->add(Element::F32(this->renderScriptObject), "", FFT_FRAME_LENGTH);

    this->fftFrameAllocation = Allocation::createTyped(this->renderScriptObject,
                                                                Type::create(this->renderScriptObject, elBuilder->create(), 1, 0, 0));

    this->melIterationAllocation = Allocation::createSized(this->renderScriptObject,
                                                                    Element::U32(this->renderScriptObject),
                                                                    MEL_BANK_FRAME_LENGTH);

    uint32_t iter[MEL_BANK_FRAME_LENGTH];
    for(uint32_t i = 0; i < MEL_BANK_FRAME_LENGTH; ++i)
        iter[i] = i;

    melIterationAllocation->copy1DFrom(iter);

    this->melBankAllocation = Allocation::createSized(this->renderScriptObject,
                                                      Element::F32(this->renderScriptObject),
                                                      MEL_BANK_FRAME_LENGTH);
}

/**
 * Calculates mel bank filters for given data.
 */
float *RSMelFilterBank::calculateMelBank(kiss_fft_cpx *fftData) {
    fftFrameAllocation->copy1DFrom(fftData);

    this->melRSinstance->set_fftFrame(fftFrameAllocation);

    this->melRSinstance->forEach_melBank(melIterationAllocation, melBankAllocation);

    renderScriptObject->finish();

    float* returnMelValues = new float[MEL_BANK_FRAME_LENGTH];

    this->melBankAllocation->copy1DTo(returnMelValues);

    return returnMelValues;
}

/**
 * Mean normalization
 */
void RSMelFilterBank::substractMean(FeatureMatrix *featuresMatrix) {
    this->melRSinstance->set_frameCount(featuresMatrix->getFramesNum());

    sp<Allocation> melCalcFramesAllocation = Allocation::createSized(renderScriptObject,
                                                                     Element::F32(renderScriptObject),
                                                                     featuresMatrix->getFramesNum() * MEL_BANK_FRAME_LENGTH);

    for(int i = 0; i < featuresMatrix->getFramesNum(); i++)
        melCalcFramesAllocation->copy1DRangeFrom(i * featuresMatrix->getFrameSize(),
                                                 featuresMatrix->getFrameSize(),
                                                 featuresMatrix->getFeaturesMatrix()[i]);

    this->melRSinstance->set_melCalculatedFrames(melCalcFramesAllocation);


    this->melRSinstance->forEach_substractMean(melIterationAllocation);

    renderScriptObject->finish();

    for(int i = 0; i < featuresMatrix->getFramesNum(); i++)
        melCalcFramesAllocation->copy1DRangeTo(i * featuresMatrix->getFrameSize(),
                                               featuresMatrix->getFrameSize(),
                                               featuresMatrix->getFeaturesMatrix()[i]);

}
