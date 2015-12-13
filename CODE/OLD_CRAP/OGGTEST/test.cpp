#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

#include "Vec2f.hpp"
#include "stb_vorbis.h"
#include "stb_vorbis.c"

#include <inttypes.h>
#include <allegro.h>

#ifndef UINT16_MAX
#define UINT16_MAX 65535
#endif

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}



struct Music {
       short* data;
       int chan, len, freq;
       
       Music()
       {
          data = NULL;
          chan = 0;
          len = 0;
          freq = 0;
       }
       
       ~Music()
       {
          free(data);
       }
};

Music loadOgg(char *filename)
{
   int error;
   stb_vorbis *v = stb_vorbis_open_filename(filename, &error, NULL);
   
   Music music;
   
   if (v)
   {
    stb_vorbis_info info = stb_vorbis_get_info(v);
    music.freq = info.sample_rate;
    
    
    music.len = stb_vorbis_decode(v, &music.chan, &music.data);
    stb_vorbis_close(v);
   }
   
   return music;
}


struct MusicPlayer {
    enum {
        RIGHT_CHAN, LEFT_CHAN
    };
    
    int buffLen, freq;
    AUDIOSTREAM* stream;
    Music* music;
    
    short* mp;
    
    static double s2f(uint16_t s)
    {
        return s * 2.0 / UINT16_MAX - 1.0;
    }

    static uint16_t f2s(double f)
    {
        return(uint16_t) ((f + 1.0) * 0.5 * UINT16_MAX);
    }
    
    MusicPlayer(int buffLen)
    {   
        this->buffLen = buffLen;
        this->freq = 44100;
        this->stream = play_audio_stream(this->buffLen, 16, 1, this->freq, 255, 128);
        this->music = NULL;
        this->mp = NULL;
    }
    
    ~MusicPlayer()
    {        
        stop_audio_stream(stream);
    }
    
    void play(Music* music)
    {
        this->music = music;
        this->mp = music->data;
    }
    
    void stop()
    {
        this->music = NULL;
        this->mp = NULL;
    }
    
    void tick()
    {
        uint16_t* buff = (uint16_t*) get_audio_stream_buffer(stream);
        
        if(buff && music)
        {
            short* end = music->data + music->len * 2;
            
            for(int i = 0; i < buffLen && mp < end; i++)
            {                
                buff[i * 2] = *(mp++) ^ 0x8000 ;
                buff[i * 2 + 1] = *(mp++) ^  0x8000 ;
            }


            free_audio_stream_buffer(stream);
        }
    }
};



int main()
{   
    bool exit = false;
    
    init();
    
    MusicPlayer mp(512);
    //SAMPLE* music = load_sample("music.wav");
    Music music = loadOgg("music.ogg");
    printf("%d %d %d %d", music.len, music.freq, music.chan, music.data);
    
    mp.play(&music);
    
    //for(int i = 44100 * 10; i < 44100 * 10 + 100; i++)
      //printf("%d", music.data[i]);

    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
       
     mp.tick();
    
     clear_to_color(buffer, 0);
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    
    deinit();
    return 0;
}END_OF_MAIN()
