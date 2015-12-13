#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include <inttypes.h>
#include <float.h>
#include <functional>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "Vec3f.hpp"
#include "DrawVec2f.hpp"
#include "Perlin.hpp"
#include "Buffer.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}


float gauss(float x, float o)
{
      float t = 2.0f * o * o;
      return 1.0f / sqrt(t * M_PI) * pow(M_E, -(x * x / t));
}

static void genGaussKernel(std::vector<float>& kernel, float r)
{
    float sum = 0.f;
    kernel.clear();
        
    for(int i = 0; i < r + 1; i++)
        kernel.push_back(gauss(i, r / 2.0f));
        
    sum = kernel[0];
    for(int i = 1; i < kernel.size(); i++)
        sum += kernel[i] * 2;
        
    for(int i = 0; i < kernel.size(); i++)
        kernel[i] /= sum;
}

int mod(int x, int n)
{
    int r = x % n;
    if(r < 0)
     r += n;
    return r;
}

void blur(BITMAP* bmp, BITMAP* temp, float rad)
{
     std::vector<float> kernel;
     
     genGaussKernel(kernel, rad);
     
     for(int y = 0; y < bmp->h; y++)
      for(int x = 0; x < bmp->w; x++)
      {
       int c0 = PIXEL(bmp, x, y);
       float r = getr32(c0) * kernel[0];
       float g = getg32(c0) * kernel[0];
       float b = getb32(c0) * kernel[0];
       
       for(int z = 1; z < kernel.size(); z++)
       {
        int c1 = PIXEL(bmp, mod(x + z, bmp->w), y);
        int c2 = PIXEL(bmp, mod(x - z, bmp->w), y);
        
        r += (getr32(c1) + getr32(c2)) * kernel[z];
        g += (getg32(c1) + getg32(c2)) * kernel[z];
        b += (getb32(c1) + getb32(c2)) * kernel[z];
       }
       
       PIXEL(temp, x, y) = makecol32(int(r), int(g), int(b));
      }
      
     for(int y = 0; y < bmp->h; y++)
      for(int x = 0; x < bmp->w; x++)
      {
       int c0 = PIXEL(temp, x, y);
       float r = getr32(c0) * kernel[0];
       float g = getg32(c0) * kernel[0];
       float b = getb32(c0) * kernel[0];

       for(int z = 1; z < kernel.size(); z++)
       {
        int c1 = PIXEL(temp, x, mod(y + z, bmp->h));
        int c2 = PIXEL(temp, x, mod(y - z, bmp->h));
        
        r += (getr32(c1) + getr32(c2)) * kernel[z];
        g += (getg32(c1) + getg32(c2)) * kernel[z];
        b += (getb32(c1) + getb32(c2)) * kernel[z];
       }
       
       PIXEL(bmp, x, y) = makecol32(int(r), int(g), int(b));
      }
}

float bolt(Perlin per1, float x)
{
	float cx = 128 + 120 * cos(x / float(SCREEN_W - 1) * 2.f * M_PI);
    float cy = 128 + 120 * sin(x / float(SCREEN_W - 1) * 2.f * M_PI);
    return 256 + clamp255((per1.getValue2f(cx / 128.f, cy / 128.f) + 1.0) * 0.5f * 128.f);
}

float sig(float x, float steepness)
{
	float d = -steepness * 0.5f;
	return 1.f / (1.f + pow(2.71f, -(x * steepness + d)));
}

void contrast(BITMAP* bmp, float c)
{
	for(int j = 0; j < bmp->h; ++j)
	  for(int i = 0; i < bmp->w; ++i)
	  {
	  	  float br = (float)getr32(PIXEL(bmp, i, j)) * c;
	  	
	  	  if(br > 255)
	  	    br = 255;
	  	
	  	  PIXEL(bmp, i, j) = makecol32(int(br), int(br), int(br));
	  }
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* bmp = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* temp = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* blurred2 = create_bitmap(SCREEN_W, SCREEN_H);
    
    clear_to_color(bmp, 0);
    Perlin per1(0.2f, 100);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
    
      if(key[KEY_SPACE])
      {
      	clear_to_color(bmp, 0);
      	per1 = Perlin(0.35f, 100);
      	
      	int lastX = 0;
    	int lastY = bolt(per1, lastX);
    
		for(int x = 1; x < SCREEN_W - 1; ++x)
      	{
      		int y = bolt(per1, x);	 
      	 	line(bmp, lastX, lastY, x, y, makecol(255, 255, 255));
      	 	lastX = x;
      	 	lastY = y;
      	}
      	
		blur(bmp, temp, 5.f);
		contrast(bmp, 20.f);
		
		draw_sprite(blurred2, bmp, 0, 0);
		blur(blurred2, temp, 15.f);
		
		for(int j = 0; j < bmp->h; ++j)
			for(int i = 0; i < bmp->w; ++i)
			{
				int b1 = getr32(PIXEL(bmp, i, j));
				int b2 = getr32(PIXEL(blurred2, i, j));
				int bs = b1 + b2;
				if(bs > 255)
					bs = 255;

				PIXEL(bmp, i, j) = makecol32(bs, bs, bs);
			}
      }
    
     clear_to_color(buffer, 0);
     draw_sprite(buffer, bmp, 0, 0);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    save_bitmap("out.bmp", bmp, NULL);
    
    destroy_bitmap(bmp);
    
    deinit();
    return 0;
}END_OF_MAIN()
