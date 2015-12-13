#include <stdio.h>
#include <conio.h>

#include <math.h>

#include <allegro.h>
#include <vector>

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]
#define M_PI 3.14159265359f
#define M_E 2.718281828459

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


int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* dest = create_bitmap(SCREEN_W, SCREEN_H);
    
    clear_to_color(buffer, 0);
    
    for(int i = 0; i < 1000; i++)
    circlefill(dest, rand() % SCREEN_W, rand() % SCREEN_H, rand() % 300,
               makecol(rand() % 255, rand() % 255, rand() % 255));
    
    blur(dest, buffer, 10.0f);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;

     clear_to_color(buffer, 0);
     
     draw_sprite(buffer, dest, 0, 0);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(dest);
    deinit();
    return 0;
}END_OF_MAIN()
