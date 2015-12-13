#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "DrawVec2f.hpp"
#include "Buffer.hpp"
#include "Col3f.hpp"
#include "Vec3f.hpp"

#include "GaussBlur.hpp"
#include "ClipRect.hpp"

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

float pow2(float x)
{
   return x * x;
}

float exposure(float v, float e)
{
    return 1.f - exp(v * -e);
}

bool drawBump(Buffer<float>& map, Vec2f const& pos, float rad)
{   
   int minx = int(pos.x - rad), miny = int(pos.y - rad),
       maxx = int(pos.x + rad), maxy = int(pos.y + rad);
   
   if(minx < 0 || miny < 0 || maxx >= map.getWidth() || maxy >= map.getHeight())
      return false;
   
   for(int y = miny; y <= maxy; y++)
      for(int x = minx; x <= maxx; x++)
      {
          Vec2f v = pos - Vec2f(x, y);
          
          //float z = MAX(0.f, pow2(rad) - v.dot(v));
          float z = sqrt(MAX(0.f, pow2(rad) - v.dot(v)));
          
          map.at(x, y) = MAX(z,map.at(x, y));
          //map.at(x, y) += z;
      }
   
   return true;
}

void genClouds(Buffer<float>& map, Vec2f pos, float rad, float spread, int n)
{
      if(n < 0 || !drawBump(map, pos + Vec2f(map.getWidth() / 2, map.getHeight() / 2), rad * randf(1.f, 1.5)))
        return;
     
     float a = 0.3, da = (M_PI - a * 2.f) / 3.f;
     float r;
     
     r = rad * randf(0.9f, 1.f  / spread);
     genClouds(map, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.7, spread * 0.9, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.2f);
     genClouds(map, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.6, spread * 0.9, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.2f);
     genClouds(map, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.6, spread * 0.9, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.f / spread);
     genClouds(map, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.7, spread * 0.9, n - 1);
     a += da * randf(0.8, 1.2);
}

float smooth(Buffer<float>& map, int x, int y)
{
   float corners = (map.at(x-1, y-1)+ map.at(x+1, y-1)+ map.at(x-1, y+1)+ map.at(x+1, y+1) ) / 16;
   float sides   = (map.at(x-1, y)  + map.at(x+1, y)  + map.at(x, y-1)  + map.at(x, y+1) ) /  8;
   float center  =  map.at(x, y) / 4;
   
   return corners + sides + center;
}

void blur(Buffer<float>& map, int iter)
{
   for(int i = 0; i < iter; i++)
    for(int y = 1; y < map.getHeight() - 1; y++)
       for(int x = 1 ; x < map.getWidth() - 1; x++)
           map.at(x, y) = smooth(map, x, y);
}

Vec3f getVertex(Buffer<float> const& heightMap, int x, int y, float k)
{
    return Vec3f(x, y, heightMap.wat(x, y) / 255.f * k);
}

void processHeightMap(Buffer<float>& heightMap, Buffer<float>& tempf)
{
   int w = heightMap.getWidth(), h = heightMap.getHeight();
   
   //blur(heightMap, 2);
   guassBlur<float>(heightMap, tempf, 4.f);
   
   for(int y = 0; y < h; y++) 
     for(int x = 0; x < w; x++)
        heightMap.at(x, y) = exposure(heightMap.at(x, y), 0.05f) * 255.f;
}

Buffer<Vec3f>& genNormalMap(Buffer<Vec3f>& normalMap, Buffer<float> const& heightMap, float k)
{
    int w = heightMap.getWidth(), h = heightMap.getHeight();
    
    for(int j = 0; j < h; j++)
       for(int i = 0; i < w; i++)
       {
          if(heightMap.at(i, j) > 15.f)
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
          
             n.normalize();
             normalMap.at(i, j) = n;
          }
          
          else
             normalMap.at(i, j) = Vec3f();
       }
    
    return normalMap;
}

void illuminated(BITMAP* target, Buffer<Vec3f> const& normalMap, Vec3f dir)
{   
    int w = normalMap.getWidth(), h = normalMap.getHeight();
    
    for(int j = 0; j < h; j++)
       for(int i = 0; i < w; i++)
       {             
          int br = clamp255(normalMap.at(i, j).dot(dir) * 255.f);
          PIXEL(target, i, j) = makecol32(br, br, br);
          
          //Vec3f n = normalMap.at(i, j);
          //Vec3f v = (Vec3f(1, 1, 1) - n) * 0.5f;
          //PIXEL(target, i, j) = makecol32(int(v.x * 255.f), int(v.y * 255.f), int(v.z * 255.f));
       }
}

void cellShaded(BITMAP* target, Buffer<Vec3f> const& normalMap, Vec3f dir)
{   
    int w = normalMap.getWidth(), h = normalMap.getHeight();
    
    ClipRect clip = ClipRect::intersect(ClipRect(0, 0, normalMap.getWidth(), normalMap.getHeight()),
                                        ClipRect(0, 0, target->w, target->h));
    
    for(int j = clip.miny; j < clip.maxy; j++)
       for(int i = clip.minx; i < clip.maxx; i++)
       {      
          Vec3f n = normalMap.at(i, j);
          
          if(n.dot(n) > 0.01f)
          {           
             int br = clamp255(normalMap.at(i, j).dot(dir) * 255.f);  
          
             if(br > 70)
                PIXEL(target, i, j) = makecol32(250, 250, 255);
             else if(br > 30)
                PIXEL(target, i, j) = makecol32(230, 230, 255);
             else
                PIXEL(target, i, j) = makecol32(200, 200, 230);
          }    
          
          else
             PIXEL(target, i, j) = 0;
       }
}


int main()
{   
    bool exit = false;
    
    init();
    
    Buffer<float> heightMap(128, 64);
    Buffer<float> tempf(heightMap.getWidth(), heightMap.getHeight());
    
    Buffer<Vec3f> normalMap(heightMap.getWidth(), heightMap.getHeight());
    
    BITMAP* cloud = create_bitmap(heightMap.getWidth(), heightMap.getHeight());
    
    bool lastSpace = false;
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     
     if(key[KEY_SPACE])
     {
         if(!lastSpace)
         {
            heightMap.fill(0);
            genClouds(heightMap, Vec2f(), 10.f, randf(0.3f, 0.7f), 3);   
            processHeightMap(heightMap, tempf);
            genNormalMap(normalMap, heightMap, 10.0f);
         }
     }
     
     lastSpace = key[KEY_SPACE];

     clear_to_color(buffer, 0);
     
     
     cellShaded(cloud, normalMap, Vec3f(5, 20, -10).unit());
     
     stretch_blit(cloud, buffer, 0, 0, cloud->w, cloud->h,
                                 SCREEN_W / 2 - cloud->w, SCREEN_H / 2 - cloud->h, cloud->w * 2, cloud->h * 2);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(cloud);
    deinit();
    return 0;
}END_OF_MAIN()
