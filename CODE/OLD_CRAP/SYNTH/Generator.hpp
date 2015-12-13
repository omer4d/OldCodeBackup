#ifndef _Generator_hpp_
#define _Generator_hpp_

#include "Module.hpp"
#include "WaveForm.hpp"
#include "DC.hpp"

class Generator: public Module {
      WaveForm waveForm;
      float amp, freq;
      
public:
       Generator(WaveForm waveForm, float amp, float freq)
       {
          this->waveForm = waveForm;
          this->amp = amp;
          this->freq = freq;
       }
       
       float sample(double time)
       {
             return waveTable[waveForm](amp, freq, time);
       }
};

#endif
