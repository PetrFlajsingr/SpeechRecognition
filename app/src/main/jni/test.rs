#pragma version(1)
#pragma rs java_package_name(cz.vutbr.fit.xflajs00.voicerecognition)

void root(const uint32_t* in, uint32_t* out){
    rsDebug("Kokot %d", *in);
    *out = 10*(*in);
}