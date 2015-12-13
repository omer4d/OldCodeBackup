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

#ifndef INT32_MIN
#define INT32_MIN (-2147483647 - 1)
#endif

#ifndef INT32_MAX
#define INT32_MAX 2147483647
#endif

#ifndef UINT32_MAX
#define UINT32_MAX 4294967295U
#endif

#define rot(x,k) (((x)<<(k))|((x)>>(32-(k))))

class PRNG {
      typedef unsigned long int  u4;
      typedef struct ranctx { u4 a; u4 b; u4 c; u4 d; } ranctx;
      
      ranctx x;
      
u4 ranval( ranctx *x ) {
    u4 e = x->a - rot(x->b, 27);
    x->a = x->b ^ rot(x->c, 17);
    x->b = x->c + x->d;
    x->c = x->d + e;
    x->d = e + x->a;
    return x->d;
}

void raninit( ranctx *x, u4 seed ) {
    u4 i;
    x->a = 0xf1ea5eed, x->b = x->c = x->d = seed;
    for (i=0; i<20; ++i) {
        (void)ranval(x);
    }
}

   public:
   PRNG(u4 seed)
   {
      raninit(&x, seed);
   }
   
   float popf(float start=0.f, float end=1.f)
   {
         u4 u = ranval(&x);
         return start + (float(u) / UINT32_MAX) * (end - start);
   }
   
   int popu(int start=0, int end=INT32_MAX)
   {
      return int(popf(start, end));
   }
};

int main()
{
    bool exit = false;
    
    init();
    
    int f = 0;
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     //++f;
     PRNG prng(f);
     
     for(int j = 0; j < SCREEN_H; ++j)
        for(int i = 0; i < SCREEN_W; ++i)
        {
           int v = prng.popu(0, 256);
           putpixel(buffer, i, j, makecol(v, v, v));
        }
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
