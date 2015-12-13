#ifndef _Util_hpp_
#define _Util_hpp_

int clamp255(float f)
{
 if(f < 0) return 0;
 if(f > 255) return 255;
 return int(f);
}

float randf()
{
      return float(rand()) / float(RAND_MAX);
}

float randf(float min, float max)
{
      return min + (max - min) * randf();
}

int round(float f)
{
    if(f < 0.0) return int(f - 0.5f);
    else return int(f + 0.5f);
}

int randi(int min, int max)
{
    return round(randf(min, max));
}

#endif
