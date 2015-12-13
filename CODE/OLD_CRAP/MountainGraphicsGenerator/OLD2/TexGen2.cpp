#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include <inttypes.h>
#include <float.h>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "Vec3f.hpp"
#include "DrawVec2f.hpp"
#include "Perlin.hpp"
#include "Buffer.hpp"

int clamp(int val, int min, int max)
{
    val = val > max ? max : val;
   return val < min ? min : val;
}

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

#define CLAMPED_PIXEL(bmp, x, y) ((long*)(bmp)->line[clamp(y, 0, bmp->h - 1)])[clamp(x, 0, bmp->w - 1)]

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

void normalizeHeightMap(Buffer<float>& map, float min, float max)
{
     for(int y = 0; y < map.getHeight(); y++)
       for(int x = 0; x < map.getWidth(); x++)
          map.at(x, y) = (map.at(x, y) - min) / (max - min);
}

float contrast(float v, float br, float cont)
{   
    v -= (1.f - br);
    if(v < 0.f) v = 0.f;
    return 1.f - pow(1 - cont, v);
}

float clampf(float x)
{
   if(x < 0.f) return 0.f;
   if(x > 1.f) return 1.f;
   return x;
}

BITMAP* genHeightMap(int w, int h)
{
    Buffer<float> map(w, h);
    
    float min = FLT_MAX, max = -FLT_MAX;
    
    for(int i = 0; i < 4; i++)
    {
     Perlin perlin1(0.4, 100);
     perlin1.setScaleX(4, w);
     perlin1.setScaleY(4, h);
     
    for(int y = 0; y < h; y++)
       for(int x = 0; x < w; x++)
       {
          float v = perlin1.getTiledValue2i(x, y);
          
          if(v < min) min = v;
          if(v > max) max = v;
          
          map.at(x, y) = fabs(map.at(x, y) - clampf((v + 1.f) * 0.5f));
       }
    }
       
    Perlin perlin1(0.5, 100);
    perlin1.setScaleX(4, w);
    perlin1.setScaleY(4, h);
    
    for(int y = 0; y < h; y++)
       for(int x = 0; x < w; x++)
       {
          float v = perlin1.getTiledValue2i(x, y);
          
          map.at(x, y) += clampf((v + 1.f) * 0.5f) * 0.2;
       }
       
    //normalizeHeightMap(map, min, max);
    
    BITMAP* bmp = create_bitmap(w, h);
    
    for(int y = 0; y < h; y++)
       for(int x = 0; x < w; x++)
       {
           //int k = clamp255(contrast(map.at(x, y), 0.8f, 0.4f) * 255.f);
           int k = clamp255(map.at(x, y) * 255.f);
           PIXEL(bmp, x, y) = makecol32(k, k, k);
       }
    
    return bmp;
}

float getHeight(BITMAP* bmp, int x, int y)
{
    int h = ((long*)(bmp)->line[wrapi(y, 0, bmp->h)])[wrapi(x, 0, bmp->w)];
    return getr32(h) / 255.f;
}

Vec3f getVertex(BITMAP* bmp, int x, int y, float k)
{
    return Vec3f(x, y, getHeight(bmp, x, y) * k);
}

BITMAP* genNormalMap(BITMAP* heightMap, float k)
{
    BITMAP* normalMap = create_bitmap(heightMap->w, heightMap->h);
    
    for(int j = 0; j < normalMap->h; j++)
       for(int i = 0; i < normalMap->w; i++)
       {
          Vec3f o = getVertex(heightMap, i, j, k);
          Vec3f a = getVertex(heightMap, i, j - 1, k);
          Vec3f b = getVertex(heightMap, i + 1, j, k);
          Vec3f c = getVertex(heightMap, i, j + 1, k);
          Vec3f d = getVertex(heightMap, i - 1, j, k);
          
          Vec3f n = (Vec3f::unitNormal(a, b, o) +
                     Vec3f::unitNormal(b, c, o) + 
                     Vec3f::unitNormal(c, d, o) + 
                     Vec3f::unitNormal(d, a, o)) * 0.25f;
                     
          Vec3f v = (Vec3f(1, 1, 1) - n) * 0.5f;
          PIXEL(normalMap, i, j) = makecol32(int(v.x * 255.f), int(v.y * 255.f), int(v.z * 255.f));
       }
       
    return normalMap;
}

BITMAP* illuminate(BITMAP* heightMap, float k, Vec3f dir)
{
    BITMAP* normalMap = create_bitmap(heightMap->w, heightMap->h);
    
    for(int j = 0; j < normalMap->h; j++)
       for(int i = 0; i < normalMap->w; i++)
       {
          
          Vec3f o = getVertex(heightMap, i, j, k);
          Vec3f a = getVertex(heightMap, i, j - 1, k);
          Vec3f b = getVertex(heightMap, i + 1, j, k);
          Vec3f c = getVertex(heightMap, i, j + 1, k);
          Vec3f d = getVertex(heightMap, i - 1, j, k);
          
          Vec3f n = (Vec3f::unitNormal(a, b, o) +
                     Vec3f::unitNormal(b, c, o) + 
                     Vec3f::unitNormal(c, d, o) + 
                     Vec3f::unitNormal(d, a, o)) * 0.25f;
                     
          int br = clamp255(n.dot(dir) * 255.f);
          
          PIXEL(normalMap, i, j) = makecol32(br, br, br);
       }
       
    return normalMap;
}

BITMAP* pixelize(BITMAP* heightMap, float k, Vec3f dir)
{
    BITMAP* normalMap = create_bitmap(heightMap->w, heightMap->h);
    
    for(int j = 0; j < normalMap->h; j++)
       for(int i = 0; i < normalMap->w; i++)
       {
          
          Vec3f o = getVertex(heightMap, i, j, k);
          Vec3f a = getVertex(heightMap, i, j - 1, k);
          Vec3f b = getVertex(heightMap, i + 1, j, k);
          Vec3f c = getVertex(heightMap, i, j + 1, k);
          Vec3f d = getVertex(heightMap, i - 1, j, k);
          
          Vec3f n = (Vec3f::unitNormal(a, b, o) +
                     Vec3f::unitNormal(b, c, o) + 
                     Vec3f::unitNormal(c, d, o) + 
                     Vec3f::unitNormal(d, a, o)) * 0.25f;
                     
          int br = clamp255(n.dot(dir) * 255.f);
          
          if(br > 110)
              PIXEL(normalMap, i, j) = makecol32(128, 128, 128);
          else if(br > 70)
              PIXEL(normalMap, i, j) = makecol32(80, 80, 80);
          else
              PIXEL(normalMap, i, j) = makecol32(32, 32, 32);
       }
       
    return normalMap;
}

float getHeight(BITMAP* bitmap, float x, float y)
{
   int xi = int(x), yi = int(y);
   float xk = x - xi, yk = y - yi;
   float h1 = getb32(CLAMPED_PIXEL(bitmap, xi, yi)),
         h2 = getb32(CLAMPED_PIXEL(bitmap, xi + 1, yi)),
         h3 = getb32(CLAMPED_PIXEL(bitmap, xi, yi + 1)),
         h4 = getb32(CLAMPED_PIXEL(bitmap, xi + 1, yi + 1));
   
   float i1 = h1 + (h2 - h1) * xk;
   float i2 = h3 + (h4 - h3) * xk;
   
   return i1 + (i2 - i1) * yk;
}

struct Color {
   int r, g, b;
   
   Color()
   {
   }
   
   Color(int c)
   {
       r = getr32(c);
       g = getg32(c);
       b = getb32(c);
   }
   
   static Color blend(Color const& c1, Color const& c2, float k)
   {
       Color c3;
       
       c3.r = int(c1.r + (c2.r - c1.r) * k);
       c3.g = int(c1.g + (c2.g - c1.g) * k);
       c3.b = int(c1.b + (c2.b - c1.b) * k);
       
       return c3;
   }
   
   int packed()
   {
       return makecol32(r, g, b);
   }
};

int getpixel(BITMAP* bitmap, float x, float y)
{
   int xi = int(x), yi = int(y);
   float xk = x - xi, yk = y - yi;
   
   Color c1(CLAMPED_PIXEL(bitmap, xi, yi)),
         c2(CLAMPED_PIXEL(bitmap, xi + 1, yi)),
         c3(CLAMPED_PIXEL(bitmap, xi, yi + 1)),
         c4(CLAMPED_PIXEL(bitmap, xi + 1, yi + 1));
   
   Color i1 = Color::blend(c1, c2, xk);
   Color i2 = Color::blend(c3, c4, xk);
   
   return Color::blend(i1, i2, yk).packed();
}

void parallax(BITMAP* dest, BITMAP* heightMap, BITMAP* colorMap, float mx, float my)
{
   for(int j = 0; j < dest->h; j++)
       for(int i = 0; i < dest->w; i++)
       {
          float tx = 0.f, ty = 0.f;
          
          int sx = int(i + tx);
          int sy = int(j + ty);
              
          if(sx < 0) sx = 0;
          if(sy < 0) sy = 0;
          if(sx > heightMap->w - 1) sx = heightMap->w - 1;
          if(sy > heightMap->h - 1) sy = heightMap->h - 1;
              
          float oldH = getr32(PIXEL(heightMap, sx, sy));
          
          for(int q = 0; q < 5; q++)
          {
              sx = int(i + tx);
              sy = int(j + ty);
              
              //if(sx < 0) sx = 0;
              //if(sy < 0) sy = 0;
              //if(sx > heightMap->w - 1) sx = heightMap->w - 1;
              //if(sy > heightMap->h - 1) sy = heightMap->h - 1;
              
              float h = (oldH + getHeight(heightMap, i + tx, j + ty)) * 0.5f;
              
              //float h = (oldH + getr32(PIXEL(heightMap, sx, sy))) * 0.5f;
              //float h = getr32(PIXEL(heightMap, sx, sy));
              
              tx = h * mx;
              ty = h * my;
              oldH = h;
          }
          
          //sx = int(i + tx);
          //sy = int(j + ty);
          
          //if(sx >= 0 && sy >= 0 && sx < colorMap->w && sy < colorMap->h)
             PIXEL(dest, i, j) = getpixel(colorMap, i + tx, j + ty); //PIXEL(colorMap, sx, sy);
       }
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* heightMap = genHeightMap(512, 512);
    BITMAP* result = illuminate(heightMap, 20.f, Vec3f(10, 20, -7).unit());
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;

     clear_to_color(buffer, 0);
     
     if(key[KEY_SPACE])
     draw_sprite(buffer, heightMap, 0, 0);
     else
     parallax(buffer, heightMap, result, (SCREEN_W / 2 - mouse_x) * 0.0005, (SCREEN_H / 2 - mouse_y) * 0.0005);
     //draw_sprite(buffer, result, 0, 0);
     
     //draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    save_bitmap("lol.bmp", result, NULL);
    
    destroy_bitmap(result);
    destroy_bitmap(heightMap);
    
    deinit();
    return 0;
}END_OF_MAIN()
