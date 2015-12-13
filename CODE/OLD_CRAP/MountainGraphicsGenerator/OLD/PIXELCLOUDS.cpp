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

struct Circle {
      Vec2f pos;
      float rad;
      
      Circle(Vec2f pos, float rad)
      {
         this->pos = pos;
         this->rad = rad;
      }
      
      Vec2f point(float rs, float a)
      {
         return pos + Vec2f(rad * rs * cos(a), rad * rs * sin(a));
      }
      
      void draw(BITMAP* buffer, int col)
      {
          //if(rad < 23)
          //DrawVec2f::circlefill(buffer, pos + Vec2f(2, 5), int(rad + 4), makecol(200, 200, 200));
          //if(rad > 15)
          DrawVec2f::circlefill(buffer, pos, int(rad), makecol(255, 255, 255));
      }
};

typedef std::list<Circle> CircleList;
typedef CircleList::iterator CircleIter;

void gen(CircleList& circleList, Vec2f pos, float rad, int n)
{
     if(n < 0)
        return;
 
     circleList.push_back(Circle(pos, rad * randf(1.f, 1.5) ));
     
     
     
     float a = 0.3, da = (M_PI - a * 2.f) / 3.f;
     float r;
     
     r = rad * randf(0.9f, 1.f + sqrt(n));
     gen(circleList, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.7, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.2f);
     gen(circleList, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.6, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.2f);
     gen(circleList, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.6, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.f + sqrt(n));
     gen(circleList, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.7, n - 1);
     a += da * randf(0.8, 1.2);
}

struct ClipRect {
       int minx, miny, maxx, maxy;
       
       void initDims(int x, int y, int w, int h)
       {
           this->minx = x;
           this->miny = y;
           this->maxx = x + ABS(w);
           this->maxy = y + ABS(h);
       }
       
       void initCorners(int x1, int y1, int x2, int y2)
       {
           this->minx = MIN(x1, x2);
           this->miny = MIN(y1, y2);
           this->maxx = MAX(x1, x2);
           this->maxy = MAX(y1, y2);
       }
       
       ClipRect()
       {
           minx = 0;
           miny = 0;
           maxx = 0;
           maxy = 0;
       }
       
       ClipRect(int x1, int y1, int x2, int y2)
       {
           initCorners(x1, y1, x2, y2);
       }
       
       int getWidth() { return maxx - minx; }
       int getHeight() { return maxy - miny; }
       
       static ClipRect intersect(ClipRect const& a, ClipRect const& b)
       {
           return ClipRect(MAX(a.minx, b.minx), MAX(a.miny, b.miny),
                           MIN(a.maxx, b.maxx), MIN(a.maxy, b.maxy));
       }
};

float pow2(float x)
{
   return x * x;
}

float exposure(float v, float e)
{
    return 1.f - exp(v * -e);
}

void drawBump(Buffer<float>& map, Vec2f const& pos, float rad)
{
   ClipRect clip = ClipRect::intersect(ClipRect(0, 0, map.getWidth(), map.getHeight()),
                                       ClipRect(int(pos.x - rad), int(pos.y - rad),
                                                int(pos.x + rad), int(pos.y + rad)));
   
   for(int y = clip.miny; y < clip.maxy; y++)
      for(int x = clip.minx; x < clip.maxx; x++)
      {
          Vec2f v = pos - Vec2f(x, y);
          
          //float z = MAX(0.f, pow2(rad) - v.dot(v));
          float z = sqrt(MAX(0.f, pow2(rad) - v.dot(v)));
          
          map.at(x, y) = MAX(z,map.at(x, y));
          //map.at(x, y) += z;
      }
}

void drawHeightmap(BITMAP* bmp, Buffer<float> const& map)
{
   ClipRect clip = ClipRect::intersect(ClipRect(0, 0, map.getWidth(), map.getHeight()),
                                       ClipRect(0, 0, bmp->w, bmp->h));
                                       
   for(int y = clip.miny; y < clip.maxy; y++)
      for(int x = clip.minx; x < clip.maxx; x++)
      {
         int br = clamp255(exposure(map.at(x, y), 0.05f) * 255.f);
         
         //int br = clamp255(map.at(x, y));
         PIXEL(bmp, x, y) = makecol(br, br, br);
      }
}

void gen2(Buffer<float>& map, Vec2f pos, float rad, int n)
{
      if(n < 0)
        return;
     
     //DrawVec2f::circlefill(bmp, pos + Vec2f(bmp->w / 2, bmp->h / 2),
     //                      int(rad * randf(1.f, 1.5)), makecol(255, 255, 255));
     
     drawBump(map, pos + Vec2f(map.getWidth() / 2, map.getHeight() / 2), rad * randf(1.f, 1.5));
     
     float a = 0.3, da = (M_PI - a * 2.f) / 3.f;
     float r;
     
     r = rad * randf(0.9f, 1.f + sqrt(n));
     gen2(map, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.7, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.2f);
     gen2(map, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.6, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.2f);
     gen2(map, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.6, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.f + sqrt(n));
     gen2(map, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.7, n - 1);
     a += da * randf(0.8, 1.2);
 
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
       {
           map.at(x, y) = smooth(map, x, y);
       }
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
                     
          n.normalize();
                     
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
              PIXEL(normalMap, i, j) = makecol32(255, 255, 255);
          else if(br > 70)
              PIXEL(normalMap, i, j) = makecol32(250, 250, 250);
          else
              PIXEL(normalMap, i, j) = makecol32(220, 220, 220);
       }
       
    return normalMap;
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
          
          n.normalize();
                     
          Vec3f v = (Vec3f(1, 1, 1) - n) * 0.5f;
          PIXEL(normalMap, i, j) = makecol32(int(v.x * 255.f), int(v.y * 255.f), int(v.z * 255.f));
       }
       
    return normalMap;
}

void gaussBlur()
{
}

int main()
{   
    bool exit = false;
    
    init();
    
    std::list<Circle> circleList, circleList2;
    Buffer<float> map(SCREEN_W, SCREEN_H), tempf(SCREEN_W, SCREEN_H);
    BITMAP* bmp = create_bitmap(SCREEN_W, SCREEN_H);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     
     if(key[KEY_SPACE])
     {
         map.fill(0);
         gen2(map, Vec2f(), 50.f, 3);
         blur(map, 70);
         //guassBlur<float>(map, tempf, 20.f);
     }


     clear_to_color(buffer, 0);
     
     //for(CircleIter i = circleList2.begin(); i != circleList2.end(); ++i)
       //i->draw(buffer, makecol(128, 128, 128));
     
     //for(CircleIter i = circleList.begin(); i != circleList.end(); ++i)
       //i->draw(buffer, makecol(255, 255, 255));
     
     
     //
     drawHeightmap(bmp, map);
     
     BITMAP* bmp2 = pixelize(bmp, 10.f, Vec3f(10, 20, -7).unit());
     //BITMAP* bmp2 = genNormalMap(bmp, 10.f);
     draw_sprite(buffer, bmp2, 0, 0);
     //destroy_bitmap(bmp2);
     
     //drawBump(buffer, Vec2f(300, 300), 255);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(bmp);
    
    deinit();
    return 0;
}END_OF_MAIN()
