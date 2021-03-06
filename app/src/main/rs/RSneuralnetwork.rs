#pragma version(1)
#pragma rs java_package_name(cz.vutbr.fit.xflajs00.voicerecognition)

// Allocations for neural network data
rs_allocation vars;
rs_allocation means;
rs_allocation biases;
rs_allocation weights;

// 0 - nothing
// 1 - sigmoid
// 2 - softmax
rs_allocation functions;

rs_allocation neuronCounts;

static float sigmoid(float* x){
    return 1.0 / (1 + exp(-(*x)));
}

float* data; //< Input data
static float dataBuffer[500]; //< Data buffer for parallel weights calculation

// Calculation of softmax divider
static float softmaxExpSumCalc(int vectorLength){
    float softmaxDivider = 0;
    for(int i = 0; i < vectorLength; i++)
        softmaxDivider += exp(data[i]);

    return softmaxDivider;
}

void calculateSoftmax(int vectorLength){
    float expSum = softmaxExpSumCalc(vectorLength);
    for(int i = 0; i < vectorLength; ++i){
        data[i] = log(exp(data[i]) / expSum);
    }
}

// Apply global means and variance
void globalMeansVars(const uint32_t* neuronIterator){
    float dataValue = data[*neuronIterator];
    // means
    dataValue = dataValue + rsGetElementAt_float(means, *neuronIterator);

    // vars
    dataValue = dataValue * rsGetElementAt_float(vars, *neuronIterator);
    data[*neuronIterator] = dataValue;
}

// Offsets for weights and biases
uint32_t weightOffset = 0;
uint32_t biasOffset = 0;

uint32_t layerNumber; //< Number of the layer currently computed

void forwardWeights(const uint32_t* neuronIterator){
    // weights
    uint32_t neuronLayerCount = rsGetElementAt_uint(neuronCounts, layerNumber);
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
    if(rsGetElementAt_int(functions, layerNumber) == 1)
        data[*neuronIterator] = sigmoid(&dataBuffer[*neuronIterator]);
    else
        data[*neuronIterator] = dataBuffer[*neuronIterator];
}