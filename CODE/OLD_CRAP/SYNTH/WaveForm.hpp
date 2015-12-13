#ifndef _WaveForm_hpp_
#define _WaveForm_hpp_

#include <math.h>

typedef float WaveFunc(float a, float f, double t);

enum WaveForm {
     SQUARE_WAVE,
     SINE_WAVE,
     SAW_WAVE1,
     SAW_WAVE2,
     TRI_WAVE,
     WAVEFORM_NUM
};

float squareWave(float a, float f, double t)
{
    float c = f * t + 0.5f;
    float k = 1.0f - c + int(c);

    return(k <= 0.5f) ? a : -a;
}

float sineWave(float a, float f, double t)
{
 return a * sin(f * 2.0f * M_PI * t);
}

float sawWave1(float a, float f, double t)
{
 float c = f * t + 0.5f;
 return 2.0f * a * (c - int(c)) - a;
}

float sawWave2(float a, float f, double t)
{
 float c = f * t;
 return 2.0f * a * (1.0f - c + int(c)) - a;
}

float triWave(float a, float f, double t)
{
 float c = f * t + 0.25f;
 float k = (1.0f - c + int(c)) * 2.0f;

 return (k <= 1.0f) ? 2.0f * a * k - a :
                      2.0f * a * (2.0f - k) - a;
}

WaveFunc* waveTable[WAVEFORM_NUM] = {&squareWave, &sineWave, &sawWave1, &sawWave2, &triWave};

#endif
