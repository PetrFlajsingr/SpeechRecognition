/*
 * Class providing interface to renderscript implementation of mel bank filters.
 *
 * Created by Petr Flajsingr on 15/12/2017.
 */


#include <RSMelFilterBank.h>
#include <constants.h>

SpeechRecognition::Feature_Extraction::RSMelFilterBank::RSMelFilterBank(const char* cacheDir) {
    this->renderScriptObject = new RS();
    this->renderScriptObject->init(cacheDir);
    this->melRSinstance = new ScriptC_RSmelfilterbank(this->renderScriptObject);

    this->prepareAllocations();
}

void SpeechRecognition::Feature_Extraction::RSMelFilterBank::prepareAllocations() {
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

float *SpeechRecognition::Feature_Extraction::RSMelFilterBank::calculateMelBank(kiss_fft_cpx *fftData) {
    fftFrameAllocation->copy1DFrom(fftData);

    this->melRSinstance->set_fftFrame(fftFrameAllocation);

    this->melRSinstance->forEach_melBank(melIterationAllocation, melBankAllocation);

    renderScriptObject->finish();

    float* returnMelValues = new float[MEL_BANK_FRAME_LENGTH];

    this->melBankAllocation->copy1DTo(returnMelValues);

    return returnMelValues;
}

void SpeechRecognition::Feature_Extraction::RSMelFilterBank::normalise(float *data) {
    for(int i = 0; i < MEL_BANK_FRAME_LENGTH; i++){
        meanForNormalisation[i] = (meanForNormalisation[i] * elementCount + data[i]) / (elementCount + 1);
        data[i] = data[i] - meanForNormalisation[i];
    }
    elementCount++;
}
