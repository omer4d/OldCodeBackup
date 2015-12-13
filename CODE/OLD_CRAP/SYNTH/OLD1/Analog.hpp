#ifndef _Analog_hpp_
#define _Analog_hpp_

#include "DC.hpp"
#include "Signal.hpp"
#include "WaveForm.hpp"

class Analog: public Signal {
      float amp, freq;
      double time;
      Signal* ampMod;
      Signal* freqMod;
      WaveForm waveForm;
      
public:

  Analog(WaveForm waveForm, float amp, float freq)
  {
     this->amp = amp;
     this->freq = freq;
     this->waveForm = waveForm;
     
     this->time = 0.0;
     this->ampMod = DC::getZero();
     this->freqMod = DC::getZero();
  }

  void setAmpMod(Signal* ampMod)
  {
     this->ampMod = ampMod;
  }
  
  void setFreqMod(Signal* freqMod)
  {
     this->freqMod = freqMod;
  }

  float sample()
  {
     return waveTable[waveForm](amp + amp * ampMod->sample(), freq, time);
  }
  
  void tick(double dt)
  {
      time += dt + dt * freqMod->sample();
  }
};

#endif
