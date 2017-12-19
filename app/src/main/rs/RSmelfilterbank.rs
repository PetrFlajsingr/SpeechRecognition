#pragma version(1)
#pragma rs java_package_name(cz.vutbr.fit.xflajs00.voicerecognition)


static const uint32_t FFT_FRAME_LENGTH = 256;
static const uint32_t MEL_BANK_FRAME_LENGTH = 24;

static const uint32_t TARGET_SAMPLING_RATE = 8000;

static const float LOW_FREQ = 64.0;
static const float HIGH_FREQ = 3800.0;


static float melFBin[FFT_FRAME_LENGTH / 2 + 1];
static float melCBin[MEL_BANK_FRAME_LENGTH + 2];
static int melCinD[MEL_BANK_FRAME_LENGTH + 2];
static float melFilterBank[FFT_FRAME_LENGTH / 2 + 1][MEL_BANK_FRAME_LENGTH];

static void initLinSpace(float* data, const float min, const float max, const uint32_t n) {
    for(int i = 0; i < n - 1; ++i)
        data[i] = min + i * (max - min) / (n - 1.0f);
    data[n - 1] = max;
}

static void melVect(float *x, const uint32_t len) {
    for(int i = 0; i < len; ++i){
        x[i] = (1127.0 * log((float)(1.0 + x[i] / 700.0)));
    }
}

static float melPoint(float x) {
    return (1127.0 * log((float)(1.0 + x /700.0)));
}

static float melInvPoint(float x) {
    return ((exp((float)(x / 1127.0)) - 1.0) * 700.0);
}

void init(){
    initLinSpace(melFBin, 0, TARGET_SAMPLING_RATE / 2, FFT_FRAME_LENGTH / 2 + 1);

    melVect(melFBin, FFT_FRAME_LENGTH / 2 + 1);

    initLinSpace(melCBin, melPoint(LOW_FREQ), melPoint(HIGH_FREQ), MEL_BANK_FRAME_LENGTH + 2);


    for(int i = 0; i < MEL_BANK_FRAME_LENGTH + 2; i++)
        melCinD[i] = floor((float)(melInvPoint(melCBin[i]) / TARGET_SAMPLING_RATE * FFT_FRAME_LENGTH)) + 1;

    for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i){
        for(int j = melCinD[i]; j < melCinD[i + 1]; ++j){
            melFilterBank[j][i] = (melCBin[i] - melFBin[j]) / (melCBin[i] - melCBin[i + 1]);
        }
        for(int j = melCinD[i + 1]; j < melCinD[i + 2]; ++j){
            melFilterBank[j][i] = (melCBin[i + 2] - melFBin[j]) / (melCBin[i + 2] - melCBin[i + 1]);
        }
    }

    if(LOW_FREQ > 0.0 && (LOW_FREQ / TARGET_SAMPLING_RATE * FFT_FRAME_LENGTH + 0.5) > melCinD[0]){
        for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i){
            melFilterBank[melCinD[0]][i] = 0.0f;
        }
    }
}

rs_allocation fftFrame;

void melBank(const uint32_t* in, float* melBankFrame) {
    *melBankFrame = 0;
    for(int j = 0; j < FFT_FRAME_LENGTH / 2; ++j){
        *melBankFrame = *melBankFrame
                + melFilterBank[j][*in]
                * ((pow((float)rsGetElementAt_float(fftFrame, j * 2), 2))
                + (pow((float)rsGetElementAt_float(fftFrame, j * 2 + 1), 2)));
    }
    if(*melBankFrame < 1){
        *melBankFrame = 0.0;
    } else{
        *melBankFrame = log(*melBankFrame);
    }
}

static float melBankFramesSum[MEL_BANK_FRAME_LENGTH]; //mean pro odecteni

uint32_t frameCount; //celkovy pocet ramcu

rs_allocation melCalculatedFrames;

void substractMean(const uint32_t* melIndex){
    melBankFramesSum[*melIndex] = 0;
    for(int frameNum = 0; frameNum < frameCount; ++frameNum) {
        melBankFramesSum[*melIndex]
            += rsGetElementAt_float(melCalculatedFrames, *melIndex + frameNum * MEL_BANK_FRAME_LENGTH);
    }

    melBankFramesSum[*melIndex] /= frameCount;

    for(int frameNum = 0; frameNum < frameCount; ++frameNum)
        rsSetElementAt_float(melCalculatedFrames,
            rsGetElementAt_float(melCalculatedFrames, *melIndex + frameNum * MEL_BANK_FRAME_LENGTH)
                - melBankFramesSum[*melIndex],
            *melIndex + frameNum * MEL_BANK_FRAME_LENGTH);
}