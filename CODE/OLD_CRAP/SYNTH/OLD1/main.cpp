#include <allegro.h>
#include <stdio.h>
#include <math.h>
#include <stdexcept>

#include <vector>

#include "Generator.hpp"

BITMAP *buffer;

void init()
{
    allegro_init();
    set_color_depth(32);

    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0))
    {
        allegro_message("%s.", allegro_error);
        exit(1);
    }

    if(install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL))
        allegro_message("Error: %s.", allegro_error);

    install_keyboard();
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

typedef unsigned short uint16;

uint16 getSample(uint16 const* data, double pos, int len)
{
    if(pos < 0) return data[0];
    else if(pos > len - 1) return data[len - 1];

    long posi = long(pos);
    double k = pos - posi;
    return uint16(data[posi] * (1.0 - k) + data[posi + 1] * k);
}

double noteFreq(double n)
{
    return pow(2, (n - 69) / 12.0) * 440.0;
}


class ModulePrototype {
protected:
   std::vector<ModulePrototype*> input;
   bool unique;
   Module* cache;
  
public:
       
   ModulePrototype(int n): input(n)
   {
      cache = NULL;
   }
   
   void setInput(int i, ModulePrototype* proto)
   {
      if(i < 0 || i > input.size() - 1)
         throw std::runtime_error("Invalid input port!");
      input[i] = proto;
   }
   
   virtual Module* createModule() = 0;
   
   Module* getModule()
   {
      if(!cache)
         cache = createModule();
      return cache;
   }
};

class GeneratorProto: public ModulePrototype {
      WaveForm waveForm;
      float amp;
      float freq;
      
public:
      GeneratorProto(WaveForm waveForm, float amp, float freq): ModulePrototype(2)
      {
         this->waveForm = waveForm;
         this->amp = amp;
         this->freq = freq;
      }
      
      Module* createModule()
      {
         Generator* gen = new Generator(waveForm, amp, freq);
         
         for(int i = 0; i < input.size(); i++)
            if(input[i])
               gen->setInput(i, input[i]->getModule());
         
         gen->setAmpMod(0);
         gen->setFreqMod(1);
         
         return gen;
      }
};

// ****************************
// * FIX LOGIC RUNNING TWICE! *
// ****************************

int main()
{
    int exit = 0;

    AUDIOSTREAM* stream;
    int buffLen = 1024, freq = 44100;
    double dt = 1.f / freq;

    init();
    
    stream = play_audio_stream(buffLen, 16, 1, freq, 255, 128);
    
    /*
    Generator gen(TRI_WAVE, 0.4f, 440.f);
    Generator gen2(SAW_WAVE1, 0.1, 10.f);
    Generator gen3(TRI_WAVE, 0.4, 1.f);
    
    gen.setInput(0, &gen2);
    gen.setInput(1, &gen3);
    gen.setFreqMod(0);
    gen.setAmpMod(1);*/
    
    
    GeneratorProto gp1(TRI_WAVE, 0.4f, 100.f);
    GeneratorProto gp2(SAW_WAVE1, 0.1, 10.f);
    GeneratorProto gp3(TRI_WAVE, 0.4, 1.f);
    GeneratorProto gp4(SQUARE_WAVE, 1.0, 0.5f);
    
    gp1.setInput(0, &gp3);
    gp1.setInput(1, &gp2);
    gp2.setInput(1, &gp4);
    gp2.setInput(0, &gp4);
    
    Module& gen = *gp1.getModule();
    
    while(!exit)
    {
        if(key[KEY_ESC])
        {
            exit = 1;
        }

        uint16* buff = (uint16*) get_audio_stream_buffer(stream);
        
        if(buff)
        {
          for(int i = 0; i < buffLen; i++)
          {
             Module::globalTick();
             gen.tick(dt);
           
             float val = gen.sample();
             buff[i * 2] = uint16(32767 + val * 32767);
             buff[i * 2 + 1] = uint16(32767 + val * 32767);
          }
          
          free_audio_stream_buffer(stream);
        }

        clear_to_color(buffer, makecol32(64, 64, 64));
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

    stop_audio_stream(stream);
    destroy_bitmap(buffer);
    return 0;
}
END_OF_MAIN()
