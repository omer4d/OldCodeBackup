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

class Modulator: public Module {
      Signal* ampMod;
      Signal* freqMod;
      Signal* base;
      
public:
       
       Modulator(): Module(3)
       {
       }
       
       void setBase(int port)
       {
            base = input.at(port) ? DC::getZero() : input.at(port);
       }
       
       void setAmpMod(int port)
       {
            ampMod = input.at(port) ? DC::getZero() : input.at(port);
       }
       
       void setFreqMod(int port)
       {
            freqMod = input.at(port) ? DC::getZero() : input.at(port);
       }
       
       float sample(double time)
       {
             
       }
};

int main()
{
    int exit = 0;

    AUDIOSTREAM* stream;
    int buffLen = 1024, freq = 44100;
    double dt = 1.0 / freq, time = 0.0;

    init();
    
    stream = play_audio_stream(buffLen, 16, 1, freq, 255, 128);
    
    Generator gen(TRI_WAVE, 0.5f, 220.f);
    Generator gen2(TRI_WAVE, 1.f, 1.f);
    
    double offset = 0.0;
    float last = 0.0;
    
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
             time += dt;
             offset += dt * gen2.sample(time);
             
             float val = gen.sample(time   + offset) + last * 0.5;
             last = val;
             
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
