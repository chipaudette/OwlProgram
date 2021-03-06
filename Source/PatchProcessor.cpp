#include "PatchProcessor.h"
#include "MemoryBuffer.hpp"
#include "device.h"
#include <string.h>
#include "ProgramVector.h"

PatchProcessor::PatchProcessor() 
  : patch(NULL), bufferCount(0) {}

PatchProcessor::~PatchProcessor(){
  clear();
}

void PatchProcessor::clear(){
  for(int i=0; i<bufferCount; ++i)
    delete buffers[i];
  bufferCount = 0;
  delete patch;
  patch = NULL;
  index = -1;
  // memset(parameterNames, 0, sizeof(parameterNames));
}

void PatchProcessor::setPatch(Patch* p){
  patch = p;
}

AudioBuffer* PatchProcessor::createMemoryBuffer(int channels, int size){
  MemoryBuffer* buf = new ManagedMemoryBuffer(channels, size);
  if(buf == NULL)
    return NULL;
  buffers[bufferCount++] = buf;
  buf->clear();
  return buf;
}

float PatchProcessor::getParameterValue(PatchParameterId pid){
  if(pid < NOF_ADC_VALUES)
    return parameterValues[pid]/4096.0f;
  else
    return 0.0f;
}

#define SMOOTH_HYSTERESIS
#define SMOOTH_FACTOR 3
void PatchProcessor::setParameterValues(uint16_t *params){
  /* Implements an exponential moving average (leaky integrator) to smooth ADC values
   * y(n) = (1-alpha)*y(n-1) + alpha*y(n)
   * with alpha=0.5, fs=48k, bs=128, then w0 ~= 18hz
   */
  if(getProgramVector()->hardware_version == OWL_MODULAR_HARDWARE){
    for(int i=0; i<NOF_ADC_VALUES; ++i)
#ifdef SMOOTH_HYSTERESIS
      if(abs(params[i]-parameterValues[i]) > 7)
#endif
    {  // invert parameter values for OWL Modular
      if(i<4)
        parameterValues[i] = (parameterValues[i]*SMOOTH_FACTOR + 4095 - params[i])/(SMOOTH_FACTOR+1);
      else
        parameterValues[i] = (parameterValues[i]*SMOOTH_FACTOR - params[i])/(SMOOTH_FACTOR+1);
    }
  }else{
    for(int i=0; i<NOF_ADC_VALUES; ++i)
#ifdef SMOOTH_HYSTERESIS
      if(abs(params[i]-parameterValues[i]) > 7)
#endif
	// 16 = half a midi step (4096/128=32)  
	parameterValues[i] = (parameterValues[i]*SMOOTH_FACTOR + params[i])/(SMOOTH_FACTOR+1);
  }
  // for(int i=NOF_ADC_VALUES; i<NOF_PARAMETERS; ++i)
  //   // todo!
}
