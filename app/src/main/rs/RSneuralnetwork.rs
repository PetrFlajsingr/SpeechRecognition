#pragma version(1)
#pragma rs java_package_name(cz.vutbr.fit.xflajs00.voicerecognition)

rs_allocation vars;
rs_allocation means;
rs_allocation biases;
rs_allocation weights;

rs_allocation neuronCounts;

static float sigmoid(float* x){
    return 1.0 / (1 + exp(-(*x)));
}

float softmaxDivider;
float* outputs;
float initSoftmax(int vectorLength){
    softmaxDivider = 0;
    for(int i = 0; i < vectorLength; i++)
        softmaxDivider += exp(outputs[i]);
}

float softmax(int index){
    return exp(outputs[index]) / softmaxDivider;
}

float* data;

void globalMeansVars(const uint32_t* neuronIterator){
    float dataValue = data[*neuronIterator];
    // means
    dataValue = dataValue + rsGetElementAt_float(means, *neuronIterator);

    // vars
    dataValue = dataValue * rsGetElementAt_float(vars, *neuronIterator);
    data[*neuronIterator] = dataValue;
}

uint32_t weightOffset = 0;
uint32_t biasOffset = 0;

uint32_t layerNumber; // pro adresovani hodnot v alokacich


static float dataBuffer[360];

void forwardWeights(const uint32_t* neuronIterator){
    // weights
    float neuronLayerCount = rsGetElementAt_uint(neuronCounts, layerNumber);
    dataBuffer[*neuronIterator] = 0;
    for(int i = 0; i < neuronLayerCount; i++){
        dataBuffer[*neuronIterator] = dataBuffer[*neuronIterator]
                    + data[i]
                    * rsGetElementAt_float(weights,
                        weightOffset + *neuronIterator * neuronLayerCount + i);
    }
}

void forwardBias(const uint32_t* neuronIterator){
    // biases
    dataBuffer[*neuronIterator] = dataBuffer[*neuronIterator] + rsGetElementAt_float(biases, biasOffset + *neuronIterator);
    if(layerNumber < 2)
        data[*neuronIterator] = sigmoid(&dataBuffer[*neuronIterator]);
    else
        data[*neuronIterator] = dataBuffer[*neuronIterator];
}