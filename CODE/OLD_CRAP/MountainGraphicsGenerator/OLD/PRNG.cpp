#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include <inttypes.h>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "Vec3f.hpp"
#include "DrawVec2f.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 800, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

int prng1(int32_t x)			 
{
    x = (x >> 13) ^ x;                     
    return (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff;
}

int prng2(int n)
{
  n = (n >> 13) ^ n;
  return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
}

void swap(int& a, int &b)
{
   int t = a;
   a = b;
   b = t;
}

int prng(int x)
{
   return x * 60493 & 0x7fffffff;
}

 #define TABLESIZE 2048

int func(int x)
{
   return (unsigned int)x & (TABLESIZE - 1);
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* heightMap = create_bitmap(512, 512);
    
    int table1[TABLESIZE];
    int table2[TABLESIZE];
    int table3[TABLESIZE];
    
    for(int i = 0; i < TABLESIZE; i++)
    {
       table1[i] = i % 256;
       table2[i] = i;
       table3[i] = i;
    }
    
    for(int i = 0; i < TABLESIZE * 3; i++)
    {
       swap(table1[rand() % TABLESIZE], table1[rand() % TABLESIZE]);
       swap(table2[rand() % TABLESIZE], table2[rand() % TABLESIZE]);
       swap(table3[rand() % TABLESIZE], table3[rand() % TABLESIZE]);
    }
    
    int z = 0;
    
    float x = -1.3;
    int xi = int(x);
    float k = x - xi;
    
    xi -= 1;
    k += 1;
    //k = 
    
    printf("%d %f", xi, k);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     if(key[KEY_SPACE]) z++;

     for(int y = 0; y < heightMap->h; y++)
       for(int x = 0; x < heightMap->w; x++)
       {           
           //int k = table1[(z + table2[(y + table3[x % TABLESIZE]) % TABLESIZE]) % TABLESIZE];
           int k = table1[(y + table2[x % TABLESIZE]) % TABLESIZE];
           PIXEL(heightMap, x, y) = makecol(k, k, k);
       }

     clear_to_color(buffer, 0);
     
     draw_sprite(buffer, heightMap, 0, 0);
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);

     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(heightMap);
    
    deinit();
    return 0;
}END_OF_MAIN()
