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
#include "Buffer.hpp"

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


//float randf(float min, float max)
//{
//      return min + (max - min) * randf();
//}

struct Perlin {
    static const int tableSize = 2048;

    float grad1d[tableSize];
    
    Perlin()
    {
        int tsize = Perlin::tableSize;
        
        for(int i = 0; i < tsize; ++i)
        	grad1d[i] = randf(-1.f, 1.f);
    }

    static float blendFunc(float k)
    {
        float k3 = k * k * k;
        float k4 = k3 * k;
        float f = 6.f * k4 * k - 15.f * k4 + 10.f * k3;

        return f;
    }

    static float blend(float v1, float v2, float k)
    {
        return v1 * blendFunc(1.0 - k) + v2 * blendFunc(k);
    }

    // ******
    // * 1D *
    // ******

    float grad(int x)
    {
        return grad1d[x % Perlin::tableSize];
    }

    float noise1f(float x)
    {
        int xi = int(x);
        float k = x - xi;

        float v1 = (k - 0.f) * grad(xi);
        float v2 = (k - 1.f) * grad(xi + 1);

        return Perlin::blend(v1, v2, k);
    }
};

void drawFunc(BITMAP* buffer, float x0, float x1, float step, float y0, int col, std::function<float(float)> f)
{
	float lastX = x0;
	float lastY = f(x0);
	
	for(float x = x0 + step; x < x1 + step; x += step)
	{
		float y = f(x);
		line(buffer, (int)lastX, (int)(lastY + y0), (int)x, (int)(y + y0), col);
		lastX = x;
		lastY = y;
	}
}

void drawFuncRoots(BITMAP* buffer, float x0, float x1, float step, float y0, int col, float error, std::function<float(float)> f)
{
	for(float x = x0; x < x1 + step; x+= step)
		if(fabs(f(x)) < error)
			circlefill(buffer, (int)x, (int)(y0 + f(x)), 2, col);
}


float sig(float x, float steepness)
{
	float d = -steepness * 0.5f;
	return 1.f / (1.f + pow(2.71f, -(x * steepness + d)));
}

void drawFunc2(BITMAP* buffer, float x0, float x1, float step, float y0, std::function<float(float)> f, std::function<float(float)> f2)
{
	float lastX = x0;
	float lastY = f(x0);
	
	for(float x = x0 + step; x < x1 + step; x += step)
	{
		int col = fabs(f2(x)) > 15.f ? makecol(255, 0, 0) : makecol(0, 0, 255);
		float y = f(x);
		line(buffer, (int)lastX, (int)(lastY + y0), (int)x, (int)(y + y0), col);
		lastX = x;
		lastY = y;
	}
}

int main()
{   
    bool exit = false;
    
    init();
	    
	Perlin perlin1;
	    
	  auto f =  [&](float x)
	  {
	  	return 100.f * perlin1.noise1f(x / 200.f);
      };
      
      auto ang = [&](float x)
      {
      	float h = 1.f;
      	float x0 = x - h, x1 = x + h;
      	float y0 = f(x0), y1 = f(x1);
      	
      	//float q = (atan2(y1 - y0, x1 - x0) / M_PI + 1) / 2;
      	
      	float q = atan2(y1 - y0, x1 - x0) / M_PI * 180;
      	
      	return q;  ///*(q * 2 - 1) * 180;*/ (sig(q, 25) * 2 - 1);
      };
      
      auto angt = [&](float x)
      {
      	
      	      	float h = 1.f;
      	float x0 = x - h, x1 = x + h;
      	float y0 = ang(x0), y1 = ang(x1);
      	
      	//float q = (atan2(y1 - y0, x1 - x0) / M_PI + 1) / 2;
      	
      	float q = atan2(y1 - y0, x1 - x0) / M_PI * 180;
      	
      	return q;
      	
      	
      	/*
      	float h = 0.1f;

		return (ang(x + h) - ang(x - h)) / (2 * h);*/
		
		
      	
      	
		  
//		float x0 = x - h, x1 = x + h;
//      	float y0 = ang(x0), y1 = ang(x1);
//      	return atan2(y1 - y0, x1 - x0) / M_PI * 180;
      };
//
//	  auto ftt = [&](float x)
//	  {
//	  	float h = 1.f;
//	  	return (f(x + h) - 2 * f(x) + f(x - h)) / (h * h) * 300.f;
//	  };
	    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     if(key[KEY_SPACE])
	 {  
       perlin1 = Perlin();
       rest(500);
   }
    
     clear_to_color(buffer, 0);

		 //line(buffer, 0, 400, 800, 400, makecol(255, 255, 255));

	 //drawFunc(buffer, 0, 800, 1, 400, makecol(255, 0, 0), f);
	 drawFunc2(buffer, 0, 800, 1, 400, f, angt);
	 
	 //drawFunc(buffer, 0, 800, 1, 400, makecol(0, 255, 0), ang);
	 
	 //drawFunc(buffer, 0, 800, 1, 400, makecol(0, 255, 0), ft);
	 
	 //drawFunc(buffer, 0, 800, 1, 400, makecol(0, 0, 255), angt);
	 
	 //drawFuncRoots(buffer, 0, 800, 1, 400, makecol(0, 0, 255), 1.f, ftt);
	 
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
