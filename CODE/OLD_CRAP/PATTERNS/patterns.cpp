#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include <vector>

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

class Perlin {
    static const int tableSize = 8;
    typedef unsigned int uint;
    
    float* grad1;
    float* grad2;
    float alpha, speed;
    
    static float blendFunc(float k)
    {
        float k3 = k * k * k;
        float k4 = k3 * k;
        float f = 6.f * k4 * k - 15.f * k4 + 10.f * k3;

        return f;
    }

    static float blend(float v1, float v2, float k)
    {
        return v1 * blendFunc(1.f - k) + v2 * blendFunc(k);
    }
    
    float rand1f(int x) const
    {
        int s = Perlin::tableSize - 1;
        return grad1[uint(x) & s] * (1.f - alpha) + grad2[uint(x) & s] * alpha;
    }
    
    void initTable(float table[], int n)
    {
        for(int i = 0; i < n; i++)
        {
            float k = float(i) / float(n - 1);
            table[i] = -1.f + 2.f * k;
            
        }
        
        /*
        for(int i = 0; i < n; i++)
        {
           table[i] += ((float(rand()) / RAND_MAX) * 2.f - 1.f) * 0.3;
        }
        
        table[rand() % n] += ((float(rand()) / RAND_MAX) * 2.f - 1.f) * 1;
        */
        
        for(int i = 0; i < n * 3; i++)
        {
            std::swap<float>(table[rand() % n], table[rand() % n]);
        } 
    } 
    
    void swap()
    {
       for(int i = 0; i < Perlin::tableSize; ++i)
          grad1[i] = grad2[i];       
    }
    
public:
    
    Perlin()
    {
        int tsize = Perlin::tableSize;
        grad1 = new float[tsize];
        grad2 = new float[tsize];
        
        initTable(grad1, tsize);
        initTable(grad2, tsize);
        alpha = 0.5f;
        speed = 0.001;
    }
    
    ~Perlin()
    {
        delete[] grad2;
        delete[] grad1;
    }
    
    void tick()
    {
       alpha += speed;
       
       if(alpha > 1.f)
       {
          swap();
          initTable(grad2, Perlin::tableSize);
          alpha = 0.f;
       }
    }
    
    float at(float x)  const
    {
        int xi = int(x);
        float k = x - xi;
        
        if(x < 0.f) xi -= 1, k += 1.f;
        
        float v1 = k * rand1f(xi);
        float v2 = (k - 1.f) * rand1f(xi + 1);
        
        return Perlin::blend(v1, v2, k);
    }
};

struct LinearMapping {
       float m, b;
       
public:
       LinearMapping(float s0, float s1, float d0, float d1)
       {
           m = 1.f / (s1 - s0) * (d1 - d0);
           b = -s0 / (s1 - s0) * (d1 - d0) + d0;
       }
       
       float map(float s)
       {
          return m * s + b;
       }
};

class Plotter {
      Perlin const& func;
      
      float rx0, rx1, ry0, ry1;
      float vx0, vx1, vy0, vy1;
      
public:
       Plotter(Perlin const& f): func(f)
       {
             rx0 = -1, rx1 = 1, ry0 = -1, ry1 = 1;
             vx0 = 0, vx1 = 300, vy0 = 300, vy1 = 0;
       }
       
       void setXRange(float x0, float x1)
       {
            rx0 = x0;
            rx1 = x1;
       }
       
       void setYRange(float y0, float y1)
       {
            ry0 = y0;
            ry1 = y1;
       }
       
       // Top left, bottom right.
       void setViewport(float x0, float y1, float x1, float y0)
       {
            vx0 = x0;
            vx1 = x1;
            vy0 = y0;
            vy1 = y1;
       }
       
       void plot(BITMAP* buffer, int n, int col)
       {
            LinearMapping lx(rx0, rx1, vx0, vx1);
            LinearMapping ly(ry0, ry1, vy0, vy1);
            
            float lastRx = rx0, lastRy = func.at(lastRx);
            
            for(int i = 1; i < n; ++i)
            {
                float rx = rx0 + (rx1 - rx0) / (n - 1) * i;
                float ry = func.at(rx);
                
                line(buffer, int(lx.map(lastRx)), int(ly.map(lastRy)), 
                             int(lx.map(rx)), int(ly.map(ry)), col);
                             
                lastRx = rx;
                lastRy = ry;
            }
            
            rect(buffer, int(lx.map(rx0)), int(ly.map(ry0)),
                         int(lx.map(rx1)), int(ly.map(ry1)), col);
       }
};

int main()
{
    bool exit = false;
    
    init();
    
    Perlin perlin;
    Plotter plotter(perlin);
    
    plotter.setYRange(-1, 1);
    plotter.setViewport(0, 0, 800, 600);
    float tx = 0.0;
  
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     plotter.setXRange(0 + tx, 16 + tx);
     //tx += 0.01;
     
     //perlin.tick();
     
     clear_to_color(buffer, makecol(64, 64, 64));
     plotter.plot(buffer, 500, makecol(255, 0, 0));
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
