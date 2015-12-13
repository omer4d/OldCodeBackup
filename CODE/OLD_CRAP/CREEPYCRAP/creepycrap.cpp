#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>

#include "Util.hpp"
#include "Col3f.hpp"
#include "FloatMap.hpp"
#include "GaussBlur.hpp"

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

typedef float FloatPalette[256];

struct ProcParams {
   FloatMap* orig;
   FloatMap* blurred1;
   FloatMap* blurred2;
   Buffer<int>* noise;
   FloatPalette* pal;
   float t;
   
   ProcParams()
   {
      orig = NULL;
      blurred1 = NULL;
      blurred2 = NULL;
      noise = NULL;
      pal = NULL;
   }
};

void proc(FloatMap& dest, ProcParams const& params)
{
   FloatMap& orig = *params.orig;
   FloatMap& blurred1 = *params.blurred1;
   FloatMap& blurred2 = *params.blurred2;
   Buffer<int>& noise = *params.noise;
   FloatPalette& pal = *params.pal;
   
   float br1 =  0.5f + (pal[100] + pal[0] + pal[33] + pal[55] + 
                       pal[140] + pal[200] + pal[17] + pal[87]) / 8.f;
   
   float br2 = 0.5f + (pal[103] + pal[3] + pal[36] + pal[58] + 
                       pal[143] + pal[203] + pal[20] + pal[90]) / 16.f;
                       
   
   for(int j = 0; j < orig.getHeight(); j++)
      for(int i = 0; i < orig.getWidth(); i++)
      {
          float A = blurred1.at(i, j) * (1.f - orig.at(i, j));
          float B = (1.f - blurred2.at(i, j)) * orig.at(i, j);
          
          float nv = pal[noise.at(i, j)];
          dest.at(i, j) = B * nv * (0.1 + 0.9 * blurred1.at(i, j))* br1 + (A * nv + A) * 0.2f * br1;
          
          /*
          float A = blurred1.at(i, j) * (1.f - blurred2.at(i, j));
          float nv = pal[noise.at(i, j)];
          
          dest.at(i, j) = (A * nv * 0.3 + A * 0.3) * br1;*/
      }
}

int main()
{   
    bool exit = false;
    
    init();
    
    
    BITMAP* test = load_bitmap("test.bmp", NULL);
    
    FloatMap orig(test->w, test->h);
    Buffer<int> noise(test->w, test->h);
    float pal[256];
    
    float t = 0.f;
    
    for(int i = 0; i < 256; i++)
      pal[i] = float(i) / float(255.f);
    
    for(int j = 0; j < test->h; j++)
       for(int i = 0; i < test->w; i++)
       {
          noise.at(i, j) = rand() % 256;
          orig.at(i, j) = getr32(PIXEL(test, i, j)) / 255.f * 0.3f +
                          getg32(PIXEL(test, i, j)) / 255.f * 0.6f + 
                          getb32(PIXEL(test, i, j)) / 255.f * 0.1f;
       }
                          
    FloatMap blurred1 = orig.deepCopy();
    FloatMap blurred2 = orig.deepCopy();
    FloatMap temp(test->w, test->h);
          
    GaussBlur::apply(blurred1, temp, 40);
    GaussBlur::apply(blurred2, temp, 10);
    blurred1.normalize(0.f, 1.f);
    blurred2.normalize(0.f, 1.f);
    
    FloatMap dest = orig.deepCopy();
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     for(int i = 0; i < 20; i++)
     {
        int x = rand() % 256;
        int y = rand() % 256;
        float t = pal[x];
        pal[x] = pal[y];
        pal[y] = t;
     }
     
     t += 0.1f;
     
     ProcParams params;
     
     params.pal = &pal;
     params.orig = &orig;
     params.blurred1 = &blurred1;
     params.blurred2 = &blurred2;
     params.noise = &noise;
     params.t = t;
     
     proc(dest, params);
     
     clear_to_color(buffer, 0);
     dest.draw(buffer);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(test);
    deinit();
    return 0;
}END_OF_MAIN()
