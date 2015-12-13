#include <stdio.h>
#include <conio.h>
#include <string>

#include <cmath>

using namespace std;

float repeatf(float f, float max)
{
   f -= floor(f / max) * max;
   return f >= 0 ? f : max + f;
}

float repeatf(float f, float min, float max)
{
   return min + repeatf(f - min, max - min);
}

float pingpong(float f, float max)
{
   f = repeatf(f, max * 2);
   return f < max ? f : max * 2.f - f;
}

int main()
{ 
  //printf("%f", repeatf(-6.1, -3, 3));
  printf("%f", pingpong(-0.1, 3));
  
  getch();
  return 0; 
}
