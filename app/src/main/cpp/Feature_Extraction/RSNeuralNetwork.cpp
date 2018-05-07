/*
 * Class providing interface to Renderscript implementation of neural network.
 * Reads neural network from storage and prepares memory for computation.
 *
 * Created by Petr Flajsingr on 17/03/2018.
 */


#include <string>
#include <RSNeuralNetwork.h>
#include <fstream>
#include <constants.h>

/**
 * Creates objects needed for Renderscript. Loads the neural network from a file given by the first
 * parameter.
 * @param filepath Path to neural network file
 * @param cacheDir cache directory for renderscript
 */
SpeechRecognition::Feature_Extraction::RSNeuralNetwork::RSNeuralNetwork(std::string filepath, const char *cacheDir) {
    this->loadFromFile(filepath);

    this->renderScriptObject = new RS();
    this->renderScriptObject->init(cacheDir);
    this->neuralNetworkRSInstance = new ScriptC_RSneuralnetwork(this->renderScriptObject);

    this->prepareAllocations();
}

/**
 * Loads neural network from file. Requires format defined in header.
 * Allocates memory.
 * @param filepath Parth to NN binary file
 */
void SpeechRecognition::Feature_Extraction::RSNeuralNetwork::loadFromFile(std::string filepath) {
    std::ifstream file;
    file.open(filepath.c_str(), std::ios::in|std::ios::binary);
    if(file.is_open()){
        this->variances = new float*[1];
        this->means = new float*[1];

        //layer count
        uint8_t layerCount;
        file.read((char*)&layerCount, 1);
        info.layerCount = layerCount;

        this->biases = new float*[info.layerCount];
        this->weights = new float**[info.layerCount];
        this->layerFunction = new ACTIVATION_FUNCTIONS[info.layerCount];

        info.neuronCounts = new unsigned int[info.layerCount];

        //variance and mean length
        uint16_t var_meanLength;
        file.read((char*)&var_meanLength, 2);

        this->variances[0] = new float[var_meanLength];
        this->means[0] = new float[var_meanLength];
        this->info.inputSize = var_meanLength;

        //mean data
        float tempFloat = 0;
        for(uint16_t i = 0; i < var_meanLength; ++i){
            file.read((char*)&tempFloat, sizeof(float));
            this->means[0][i] = tempFloat;
        }

        //variance data
        for(uint16_t i = 0; i < var_meanLength; ++i){
            file.read((char*)&tempFloat, sizeof(float));
            this->variances[0][i] = tempFloat;
        }

        uint8_t temp_8b;
        for(uint16_t layerIterator = 0; layerIterator < layerCount; layerIterator++){
            file.read((char*)&temp_8b, 1);

            uint16_t biasSize;
            file.read((char*)&biasSize, 2);
            info.neuronCounts[layerIterator] = biasSize;

            this->biases[layerIterator] = new float[biasSize];

            //bias data
            for(uint16_t i = 0; i < biasSize; ++i){
                file.read((char*)&tempFloat, sizeof(float));
                this->biases[layerIterator][i] = tempFloat;
            }

            uint16_t weightSizeX;
            file.read((char*)&weightSizeX, 2);
            uint16_t weightSizeY;
            file.read((char*)&weightSizeY, 2);

            this->weights[layerIterator] = new float*[weightSizeX];
            //weights data
            for(uint16_t i = 0; i < weightSizeX; ++i){
                this->weights[layerIterator][i] = new float[weightSizeY];
                for(uint16_t j = 0; j < weightSizeY; ++j) {
                    file.read((char *) &tempFloat, sizeof(float));
                    this->weights[layerIterator][i][j] = tempFloat;
                }
            }

            file.read((char*)&temp_8b, 1);
            switch(temp_8b){
                case 0:
                    this->layerFunction[layerIterator] = NOTHING;
                    break;
                case 1:
                    this->layerFunction[layerIterator] = SIGMOID;
                    break;
                case 2:
                    this->layerFunction[layerIterator] = SOFTMAX;
                    break;
                default:break;
            }
        }
        file.close();
    }
}

/**
 * Prepares Renderscript allocations and copies data to them.
 */
void SpeechRecognition::Feature_Extraction::RSNeuralNetwork::prepareAllocations() {
    // Allocation for means - Renderscript
    sp<Allocation> meansAllocation = Allocation::createSized(this->renderScriptObject,
                                                             Element::F32(this->renderScriptObject),
                                                             info.neuronCounts[0]);
    meansAllocation->copy1DFrom(this->means[0]);
    neuralNetworkRSInstance->set_means(meansAllocation);
    //\
    //Alocation for variance - Renderscript
    sp<Allocation> varsAllocation = Allocation::createSized(this->renderScriptObject,
                                                            Element::F32(this->renderScriptObject),
                                                            info.neuronCounts[0]);
    varsAllocation->copy1DFrom(this->variances[0]);
    neuralNetworkRSInstance->set_vars(varsAllocation);
    //\
    //Allocation for biases - Renderscript
    sp<Allocation> biasesAllocation = Allocation::createSized(this->renderScriptObject,
                                                              Element::F32(this->renderScriptObject),
                                                              getTotalNeuronCount());
    unsigned int offset = 0;
    for(int i = 0; i < info.layerCount; ++i) {
        biasesAllocation->copy1DRangeFrom(offset,
                                          info.neuronCounts[i],
                                          this->biases[i]);
        offset += info.neuronCounts[i];
    }

    neuralNetworkRSInstance->set_biases(biasesAllocation);
    //\
    //Allocation for weights - Renderscript
    sp<Allocation> weightsAllocation = Allocation::createSized(this->renderScriptObject,
                                                               Element::F32(this->renderScriptObject),
                                                               getTotalWeightsCount());

    offset = 0;
    uint32_t lastLayer;
    for(int i = 0; i < info.layerCount; ++i){
        if(i == 0)
            lastLayer = info.inputSize;
        else
            lastLayer = info.neuronCounts[i - 1];
        for(int j = 0; j < info.neuronCounts[i]; ++j){
            weightsAllocation->copy1DRangeFrom(offset + j * lastLayer,
                                               lastLayer,
                                               this->weights[i][j]);
        }
        offset += lastLayer * info.neuronCounts[i];
    }

    neuralNetworkRSInstance->set_weights(weightsAllocation);
    //\
    //Allocation for neuron counters - Renderscript
    sp<Allocation> neuronCountAllocation = Allocation::createSized(this->renderScriptObject,
                                                                   Element::U32(this->renderScriptObject),
                                                                   info.layerCount);
    unsigned int neuronCountsToRS[info.layerCount];
    neuronCountsToRS[0] = info.inputSize;
    for(int i = 1;i < info.layerCount + 1; ++i) {
        neuronCountsToRS[i] = info.neuronCounts[i - 1];
    }
    neuronCountAllocation->copy1DRangeFrom(0, info.layerCount, neuronCountsToRS);
    neuralNetworkRSInstance->set_neuronCounts(neuronCountAllocation);


    //\
    //Allocation for activation functions - Renderscript
    sp<Allocation> layerFunctionAllocation = Allocation::createSized(this->renderScriptObject,
                                                                     Element::U32(this->renderScriptObject),
                                                                     info.layerCount);

    layerFunctionAllocation->copy1DRangeFrom(0, info.layerCount, layerFunction);
    neuralNetworkRSInstance->set_functions(layerFunctionAllocation);

    dataAllocation = Allocation::createSized(this->renderScriptObject,
                                                            Element::F32(this->renderScriptObject),
                                                            500);

    for(int i = 0; i < 500; i++) {
        neuronIterator[i] = i;
    }
}

/**
 * Prepares input for NN.
 * @param data input data - mel bank results
 * @param index index of middle frame
 * @param out
 */
float* SpeechRecognition::Feature_Extraction::RSNeuralNetwork::prepareInput(FeatureMatrix *data, int index) {
    const int ROLLING_WINDOW_SIZE = 7;
    float* result = new float[info.inputSize];
    int frameNum = 0;
    for(int i = 0; i < ROLLING_WINDOW_SIZE * 2 + 1; i++){
        for(int j = 0; j < MEL_BANK_FRAME_LENGTH; j++){
            frameNum = index + i - ROLLING_WINDOW_SIZE;
            if(frameNum < 0){
                frameNum = 0;
            }
            if(frameNum > data->getHeight() - 1){
                frameNum = data->getHeight() - 1;
            }
            result[i + (ROLLING_WINDOW_SIZE * 2 + 1) * j] = data->getFeaturesMatrix()[frameNum][j];
        }
    }
    return result;
}

/**
 * Pass data through network.
 * @param data input data
 * @return clearOutputNode of neural network
 */
float *SpeechRecognition::Feature_Extraction::RSNeuralNetwork::forward(float *data) {
    dataAllocation->copy1DFrom(data);

    iterationAllocation = Allocation::createSized(this->renderScriptObject,
                                        Element::U32(this->renderScriptObject),
                                        info.inputSize);
    iterationAllocation->copy1DFrom(neuronIterator);

    this->neuralNetworkRSInstance->bind_data(dataAllocation);

    //globals
    this->neuralNetworkRSInstance->forEach_globalMeansVars(iterationAllocation);

    renderScriptObject->finish();

    uint32_t biasOffset = 0, weightOffset = 0;
    for(uint32_t layerIterator = 0; layerIterator < info.layerCount; layerIterator++){
        iterationAllocation = Allocation::createSized(this->renderScriptObject,
                                            Element::U32(this->renderScriptObject), info.neuronCounts[layerIterator]);

        iterationAllocation->copy1DFrom(neuronIterator);

        this->neuralNetworkRSInstance->set_layerNumber(layerIterator);
        this->neuralNetworkRSInstance->set_biasOffset(biasOffset);
        this->neuralNetworkRSInstance->set_weightOffset(weightOffset);
        this->neuralNetworkRSInstance->forEach_forwardWeights(iterationAllocation);
        this->renderScriptObject->finish();


        this->neuralNetworkRSInstance->forEach_forwardBias(iterationAllocation);
        renderScriptObject->finish();

        biasOffset += info.neuronCounts[layerIterator];
        if(layerIterator == 0){
            weightOffset += info.inputSize * info.neuronCounts[layerIterator];
        } else{
            weightOffset += info.neuronCounts[layerIterator - 1] * info.neuronCounts[layerIterator];
        }
    }

    neuralNetworkRSInstance->invoke_calculateSoftmax(info.neuronCounts[info.layerCount - 1]);

    float* result = new float[info.neuronCounts[info.layerCount - 1]];
    dataAllocation->copy1DRangeTo(0, info.neuronCounts[info.layerCount - 1], result);

    return result;
}

/**
 * Forwards entire feature matrix.
 * @param data clearOutputNode of mel bank filters
 * @return matrix of neural network outputs
 */
SpeechRecognition::Utility::FeatureMatrix *SpeechRecognition::Feature_Extraction::RSNeuralNetwork::forwardAll(FeatureMatrix *data) {
    FeatureMatrix* result = new FeatureMatrix();
    result->initialize(data->getHeight(), 46);

    float* inputData;
    for(int frameNum = 0; frameNum < data->getHeight(); frameNum++){
        inputData = prepareInput(data, frameNum);
        result->getFeaturesMatrix()[frameNum] = this->forward(inputData);
    }

    return result;
}

/**
 * Total count of neurons in network. For memory allocation and offset calculation.
 */
unsigned int SpeechRecognition::Feature_Extraction::RSNeuralNetwork::getTotalNeuronCount() {
    if(totalNeuronCount == 0) {
        for(int i = 0; i < info.layerCount; ++i) {
            this->totalNeuronCount += info.neuronCounts[i];
        }
    }
    return totalNeuronCount;
}

/**
 * Total count of weights in network. For memory allocation and offset calculation.
 */
unsigned int SpeechRecognition::Feature_Extraction::RSNeuralNetwork::getTotalWeightsCount() {
    unsigned int total = info.inputSize * info.neuronCounts[0];
    for(int i = 0; i < info.layerCount - 1; ++i) {
        total += info.neuronCounts[i] * info.neuronCounts[i + 1];
    }
    return total;
}


SpeechRecognition::Feature_Extraction::RSNeuralNetwork::~RSNeuralNetwork() {
    for(unsigned int i = 0; i < info.layerCount; ++i){
        delete[] this->biases[i];
        if(i == 0){
            for(unsigned int j = 0; j < info.inputSize; ++j) {
                delete[] this->weights[i][j];
            }
        } else{
            for(unsigned int j = 0; j < info.neuronCounts[i]; ++j) {
                delete[] this->weights[i][j];
            }
        }
        delete[] this->weights[i];
    }
    delete[] this->variances[0];
    delete[] this->means[0];

    delete[] this->biases;
    delete[] this->variances;
    delete[] this->means;
    delete[] this->weights;
    delete[] this->layerFunction;


    delete neuralNetworkRSInstance;
}

