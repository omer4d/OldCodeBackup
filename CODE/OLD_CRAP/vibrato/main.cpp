#include <allegro.h>
#include <stdio.h>
#include <math.h>

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

float squareWave(float a, float f, float t)
{
    float c = f * t + 0.5f;
    float k = 1.0f - c + int(c);

    return(k <= 0.5f) ? a : -a;
}

float sineWave(float a, float f, float t)
{
 return a * sin(f * 2.0f * M_PI * t);
}

float sawWave1(float a, float f, float t)
{
 float c = f * t + 0.5f;
 return 2.0f * a * (c - int(c)) - a;
}

float sawWave2(float a, float f, float t)
{
 float c = f * t;
 return 2.0f * a * (1.0f - c + int(c)) - a;
}

float triWave(float a, float f, float t)
{
 float c = f * t + 0.25f;
 float k = (1.0f - c + int(c)) * 2.0f;

 return (k <= 1.0f) ? 2.0f * a * k - a :
                      2.0f * a * (2.0f - k) - a;
}

float lin(float x1, float y1, float x2, float y2, float x)
{
    return(x - x1) * (y2 - y1) / (x2 - x1) + y1;
}

float graph(float x[], float y[], int n, float k)
{
    int i;
    for(i = 0; i < n && x[i] < k; i++);

    if(i == 0 || x[i] < k) return x[i];
    else
        return lin(x[i - 1], y[i - 1], x[i], y[i], k);
}

float specWave(float a, float f, float t, float q)
{
    float c = f * t + 0.5f;
    float k = 1.0f - c + int(c);
    float p = 0.5 - q;

    float x[] = {0, q, q + p, 2 * q + p, 1.0};
    float y[] = {a, a, -a, -a, a};

    return graph(x, y, 5, k);
}

int main()
{
    int exit = 0;

    init();

    SAMPLE* src = load_sample("test.wav");
    SAMPLE* dest = create_sample(16, 1, src->freq, src->len);
    
    uint16* srcData = (uint16*)src->data;
    uint16* srcDataLeft = new uint16[src->len];
    uint16* srcDataRight = new uint16[src->len];
    
    uint16* destData = (uint16*)dest->data;
    
    for(int i = 0; i < src->len; i++)
    {
        srcDataLeft[i] = srcData[i * 2];
        srcDataRight[i] = srcData[i * 2 + 1];
    }

    double realt = 0.0, t = 0.0, dt = 1.0 / src->freq;
    
    for(int i = 0; i < src->len; i++)
    {    
        destData[i * 2] = getSample(srcDataLeft, t * src->freq, src->len);
        destData[i * 2 + 1] = getSample(srcDataRight, t * src->freq, src->len);
        
        t += dt + dt * (1.0 + specWave(1.0, 3.5, realt, 0.3)) * 0.5 * (noteFreq(2) / noteFreq(0) - 1.0);
        realt += dt;
    }
    
    //sin(2.0 * M_PI * realt * 3.0)
    
    play_sample(dest, 255, 128, 1000, 1);

    while(!exit)
    {
        if(key[KEY_ESC])
        {
            exit = 1;
        }

        clear_to_color(buffer, makecol32(64, 64, 64));
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

    destroy_sample(dest);
    destroy_sample(src);
    destroy_bitmap(buffer);
    return 0;
}
END_OF_MAIN()
