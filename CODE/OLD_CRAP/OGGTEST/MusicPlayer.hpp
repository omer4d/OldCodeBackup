/* 
 * File:   MusicPlayer.hpp
 * Author: Stas
 *
 * Created on February 7, 2013, 2:01 PM
 */

#ifndef MUSICPLAYER_HPP
#define	MUSICPLAYER_HPP

#include <inttypes.h>
#include <allegro.h>

#ifndef UINT16_MAX
#define UINT16_MAX 65535
#endif

struct Band {
    float fmin, fmax, energy;
    
    Band(float fmin, float fmax)
    {
        this->fmin = fmin;
        this->fmax = fmax;
        this->energy = 0.f;
    }
};

struct MusicPlayer {
    enum {
        RIGHT_CHAN, LEFT_CHAN
    };
    
    int buffLen, freq;
    AUDIOSTREAM* stream;
    SAMPLE* music;
    
    uint16_t* mp;
    
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
    
    void play(SAMPLE* music)
    {
        this->music = music;
        this->mp = (uint16_t*)music->data;
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
            uint16_t* end = (uint16_t*)music->data + music->len * 2;
            
            for(int i = 0; i < buffLen && mp < end; i++)
            {                
                buff[i * 2] = *(mp++);
                buff[i * 2 + 1] = *(mp++);
            }


            free_audio_stream_buffer(stream);
        }
    }
};

#endif	/* MUSICPLAYER_HPP */

