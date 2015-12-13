#ifndef _Generator_hpp_
#define _Generator_hpp_

#include "Module.hpp"
#include "Analog.hpp"
#include "WaveForm.hpp"
#include "DC.hpp"

class Generator: public Module {
      Analog wave;
      
public:
       Generator(WaveForm waveForm, float amp, float freq): Module(2), wave(waveForm, amp, freq)
       {
       }
       
       void onTick(double dt)
       {
            wave.tick(dt);
       }
       
       float sample()
       {
             return wave.sample();
       }
       
       void setAmpMod(int port)
       {
            if(input[port] == NULL)
               wave.setAmpMod(DC::getZero());
            else
               wave.setAmpMod(input[port]);
       }
       
       void setFreqMod(int port)
       {
            if(input[port] == NULL)
               wave.setFreqMod(DC::getZero());
            else
               wave.setFreqMod(input[port]);
       }
};

#endif
