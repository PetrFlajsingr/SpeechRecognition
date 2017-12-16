#pragma version(1)
#pragma rs java_package_name(cz.vutbr.fit.xflajs00.voicerecognition)

rs_allocation vars;
rs_allocation means;
rs_allocation biases;
rs_allocation weights;

rs_allocation neuronCounts;

rs_allocation data;

void forwardInputLayer(const uint32_t* neuronIterator){
    float dataValue = rsGetElementAt_float(data, *neuronIterator);
    // means
    dataValue = dataValue + rsGetElementAt_float(means, *neuronIterator);

    // vars
    dataValue = dataValue * rsGetElementAt_float(vars, *neuronIterator);

    // weights
    float outputValue = 0;
    for(int i = 0; i < rsGetElementAt_float(neuronCounts, 1) ; i++){
        outputValue = outputValue + dataValue * rsGetElementAt_float(weights, *neuronIterator + i);
    }

    // biases
    rsSetElementAt_float(data,
            outputValue + rsGetElementAt_float(biases, *neuronIterator),
             *neuronIterator);
}

uint32_t layerNumber; // pro adresovani hodnot v alokacich
void forwardLayer(const uint32_t* neuronIterator){
}