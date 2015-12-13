#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <algorithm>
#include <list>
#include <float.h>

#include <vector>

#include "DrawVec2f.hpp"

#include "Vec2f.hpp"
#include "Util.hpp"

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

struct AABB {
 Vec2f min, max;
};

    int w = 32, h = 32;

struct Poly {
  std::vector<Vec2f> vertexVec;
  
  enum VertexOrder {
        CW, CCW
    };
    

  Poly()
  {
  }
  
  Poly(int n, float cx, float cy, float rx, float ry, float angOffs)
  {
     for(int i = 0; i < n; i++)
     {
        float ang = (angOffs / 180.f * M_PI) + float(i) / float(n) * M_PI * 2.f;
        Vec2f p(cx + rx * cos(ang), cy + ry * sin(ang));
        vertexVec.push_back(p);
     }
  }
  
  static Poly rectByCenter(float cx, float cy, float w, float h)
  {
     Poly poly;
     
     poly.vertexVec.push_back(Vec2f(cx - w * 0.5, cy - h * 0.5));
     poly.vertexVec.push_back(Vec2f(cx + w * 0.5, cy - h * 0.5));
     poly.vertexVec.push_back(Vec2f(cx + w * 0.5, cy + h * 0.5));
     poly.vertexVec.push_back(Vec2f(cx - w * 0.5, cy + h * 0.5));
     
     return poly;
  }
  
  static Poly rectByCorner(float minx, float miny, float w, float h)
  {
   Poly poly;
   
   poly.vertexVec.push_back(Vec2f(minx, miny));
   poly.vertexVec.push_back(Vec2f(minx + w, miny));
   poly.vertexVec.push_back(Vec2f(minx + w, miny + h));
   poly.vertexVec.push_back(Vec2f(minx, miny + h));
   
   return poly;
  }
  
  void draw(BITMAP* buffer, int col)
  {
     int* points = new int[vertexVec.size() * 2];
     
     for(int i = 0; i < vertexVec.size(); i++)
     {
       points[i * 2]     = int(vertexVec[i].x + 0.5f);
       points[i * 2 + 1] = int(vertexVec[i].y + 0.5f);
     }
     
     polygon(buffer, vertexVec.size(), points, col);
     
     delete[] points;
  }
  
    void draw(BITMAP* buffer, int dx, int dy, int col)
  {
     int* points = new int[vertexVec.size() * 2];
     
     for(int i = 0; i < vertexVec.size(); i++)
     {
       points[i * 2]     = int(dx + vertexVec[i].x + 0.5f);
       points[i * 2 + 1] = int(dy + vertexVec[i].y + 0.5f);
     }
     
     polygon(buffer, vertexVec.size(), points, col);
     
     delete[] points;
  }
  
      float signedArea()
    {
        float s = 0;

        for(int i = 0; i < vertexVec.size(); i++)
        {
            int j = (i + 1) % vertexVec.size();
            s += vertexVec[i].x * vertexVec[j].y - vertexVec[j].x * vertexVec[i].y;
        }

        s *= 0.5;
        return s;
    }
    
    void reverseVertexOrder()
    {
        std::reverse(vertexVec.begin(), vertexVec.end());
    }
    
    void setVertexOrder(VertexOrder order)
    {
        float sa = signedArea();
        if((sa >= 0 && order == CCW) || (sa < 0 && order == CW))
            reverseVertexOrder();
    }
    
    AABB aabb()
    {
       AABB box;
       box.min.x = FLT_MAX;
       box.min.y = FLT_MAX;
       box.max.x = -FLT_MAX;
       box.max.y = -FLT_MAX;
       
       for(int i = 0; i < vertexVec.size(); i++)
       {
          if(vertexVec[i].x < box.min.x) box.min.x = vertexVec[i].x;
          if(vertexVec[i].y < box.min.y) box.min.y = vertexVec[i].y;
          if(vertexVec[i].x > box.max.x) box.max.x = vertexVec[i].x;
          if(vertexVec[i].y > box.max.y) box.max.y = vertexVec[i].y;
       }
       
       return box;
    }
    
    void repos()
    {
         AABB box = aabb();
         
         int tx = int(box.min.x / w) * w;
        int ty = int(box.min.y / h) * h;
         
         for(int i = 0; i < vertexVec.size(); i++)
         {
            vertexVec[i].x -= tx;
            vertexVec[i].y -= ty;
         }
    }
    
    void removeDups()
    {
         for(std::vector<Vec2f>::iterator j = vertexVec.begin(); j != vertexVec.end(); ++j)
         {
            std::vector<Vec2f>::iterator i = j;
            ++i;
            
            while(i != vertexVec.end())
            {
              if(Vec2f::squaredDist(*i, *j) < 0.1)
              {
                printf("!");
                i = vertexVec.erase(i);
              }
              else
               ++i;
            }    
         }
    }
};

struct Ray {
  Vec2f orig, dir;
  
  void init(Vec2f const& src, Vec2f const& dest)
  {
     orig = src;
     dir = (dest - src).unit();
  }
  
  float dist(Vec2f const& p) const
  {
     if(fabs(dir.x) > 0.01)
        return (p.x - orig.x) / dir.x;
     else
        return (p.y - orig.y) / dir.y;
  }
  
  Vec2f point(float k) const
  {
     return orig + dir * k;
  }
};

float area(Vec2f const& a, Vec2f const& b, Vec2f const& c)
{
   Vec2f min, max;
   min.x = MIN(MIN(a.x, b.x), c.x);
   min.y = MIN(MIN(a.y, b.y), c.y);
   max.x = MAX(MAX(a.x, b.x), c.x);
   max.y = MAX(MAX(a.y, b.y), c.y);
   
   return (max.x - min.x) * (max.y - min.y) -
          (fabs((a.x - b.x) * (a.y - b.y)) +
           fabs((b.x - c.x) * (b.y - c.y)) +
           fabs((c.x - a.x) * (c.y - a.y))) * 0.5f;
}

static bool clip(Vec2f& p, Vec2f const& v1, Vec2f const& v2, Ray const& ray)
{
  Vec2f n = ray.dir.normal();
  float d1 = (v1 - ray.orig).dot(n);
  float d2 = (v2 - ray.orig).dot(n);
  
  if((d1 < 0 && d2 >= 0) || (d2 < 0 && d1 >= 0))
  {
     float k = fabs(d1) / (fabs(d1) + fabs(d2));
     p = v1 + (v2 - v1) * k;
     return true;
  }
  
  return false;
}

bool testPoly(Poly& poly)
{
  if(poly.vertexVec.size() < 3)
    return false;
  else if(poly.vertexVec.size() == 3)
    return area(poly.vertexVec[0], poly.vertexVec[1], poly.vertexVec[2]) > 10.f;
  else if(poly.vertexVec.size() == 4)
    return (area(poly.vertexVec[0], poly.vertexVec[1], poly.vertexVec[2]) + 
            area(poly.vertexVec[2], poly.vertexVec[3], poly.vertexVec[0])) > 10.f;
  else
    return true;
}

bool clipPoly(Poly& dest1, Poly& dest2, Poly const& src, Ray const& ray)
{
     Poly* curr = &dest1;
     Poly* other = &dest2;
     
     for(int i = 0; i < src.vertexVec.size(); i++)
     {
        int n = src.vertexVec.size();
        Vec2f p;
        
        curr->vertexVec.push_back(src.vertexVec[i]);
        
        if(clip(p, src.vertexVec[i], src.vertexVec[(i + 1) % n], ray))
        {
           curr->vertexVec.push_back(p);
           other->vertexVec.push_back(p);
           Poly* temp = curr;
           curr = other;
           other = temp;
        }
     }
     
  return testPoly(dest1) && testPoly(dest2);
}

#define ROWS 6
#define COLS 6

#define HFLIP 0x1
#define VFLIP 0x2
#define DFLIP 0x4

Vec2f transform(Vec2f v, int flags)
{
    Vec2f res;
 
 			bool hflip = (flags & HFLIP) != 0;
				bool vflip = (flags & VFLIP) != 0;
				bool dflip = (flags & DFLIP) != 0;
				
				/*
				float nx = hflip ? (w - v.x) : v.x;
				float ny = vflip ? (h - v.y) : v.y;
				
				res.x = dflip ? ny : nx;
				res.y = dflip ? nx : ny;*/
				
				float nx = dflip ? v.y : v.x;
				float ny  =dflip ? v.x : v.y;
				
			 res.x = hflip ? (w - nx) : nx;
				res.y = vflip ? (h - ny) : ny;
				
				return res;
}

int main()
{
    bool exit = false;
    
    init();
    

    
    std::vector<Poly> polyVec;
    
    
    polyVec.push_back(Poly(4, w, h, w, h, 0.0));
    polyVec.push_back(Poly(4, w * 4, h, w * 2, h, 0.0));
    polyVec.push_back(Poly(4, w * 3, h * 3, w * 3, h, 0.0));

    polyVec.push_back(Poly::rectByCorner(0, 4 * h, w, h));
    polyVec.push_back(Poly::rectByCorner(w, 5 * h, w, -h * 2.f/3.f));
    polyVec.push_back(Poly::rectByCorner(2*w, 5 * h, w, -h * 1.f/3.f));
    
    polyVec.push_back(Poly::rectByCorner(0, 5 * h, w, h));
    polyVec.push_back(Poly::rectByCorner(w, 5 * h, w, h*2.f/3.f));
    polyVec.push_back(Poly::rectByCorner(2*w, 5 * h, w, h*1.f/3.f));
    
    polyVec.push_back(Poly::rectByCorner(4*w, 4 * h, -w*2.f/3.f, h));
    polyVec.push_back(Poly::rectByCorner(4*w, 5 * h, -w*1.f/3.f, h));
    
    polyVec.push_back(Poly::rectByCorner(4*w, 4 * h, w*2.f/3.f, h));
    polyVec.push_back(Poly::rectByCorner(4*w, 5 * h, w*1.f/3.f, h));

    polyVec.push_back(Poly::rectByCenter(5.5f*w, 4.5 * h, w*0.5, h));
    polyVec.push_back(Poly::rectByCenter(5.5f*w, 5.5 * h, w, h*0.5));


    int n = polyVec.size();
    
    
    for(int i = 0; i < n; i++)
    {
     
     for(int x = 0; x < SCREEN_W; x += w)
     {
        Ray ray;
        ray.init(Vec2f(x, 0), Vec2f(x, SCREEN_H));
      
        Poly poly1, poly2;
        if(clipPoly(poly1, poly2, polyVec[i], ray))
        {
          polyVec[i] = poly1;
          polyVec.push_back(poly2);
        }
      }
    }

    
    n = polyVec.size();
    for(int i = 0; i < n; i++)
    {
      for(int y = 0; y < SCREEN_H; y += h)
      {
        Ray ray;
        ray.init(Vec2f(0, y), Vec2f(SCREEN_W, y));
      
        Poly poly1, poly2;
        if(clipPoly(poly1, poly2, polyVec[i], ray))
        {
          polyVec[i] = poly1;
          polyVec.push_back(poly2);
        }
      }
    }
    
    

    std::vector<int> colVec;
    for(int i = 0; i < polyVec.size(); i++)
    {
       colVec.push_back(makecol(128, 0, 0));
       //colVec.push_back(makecol(100 + rand() % 150, 100 + rand() % 150, 100 + rand() % 150));
    }
    
    printf("%d", polyVec.size());

    BITMAP* sheet = create_bitmap(w * COLS, h * ROWS);
    
    clear_to_color(sheet, 0); 
     
     for(int i = 0; i < polyVec.size(); i++)
        polyVec[i].draw(sheet, colVec[i]);
    
    save_bitmap("collider_sheet.bmp", sheet, NULL);
    destroy_bitmap(sheet);



    Poly* arr[COLS][ROWS];
    for(int j = 0; j < ROWS; j++)
      for(int i = 0; i < COLS; i++)
        arr[i][j] = NULL;
    
    
    for(int i = 0; i < polyVec.size(); i++)
    {
       polyVec[i].removeDups();
       polyVec[i].setVertexOrder(Poly::CW);
       AABB aabb = polyVec[i].aabb();

       int tx = int(aabb.min.x / w);
       int ty = int(aabb.min.y / h);
       arr[tx][ty] = &polyVec[i];
       
       polyVec[i].repos();
    }


/*
    for(int i = 0; i < polyVec.size(); i++)
    {
       for(int j = 0; j< polyVec[i].vertexVec.size(); j++)
         polyVec[i].vertexVec[j] = transform(polyVec[i].vertexVec[j], DFLIP | HFLIP);
    }*/

    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, 0); 
     
     for(int j = 0; j < ROWS; j++)
       for(int i = 0; i < COLS; i++)
       if(arr[i][j] != NULL)
       {
         arr[i][j]->draw(buffer, i * w, j * h, makecol(128, 0, 0));
       }
     
     //for(int i = 0; i < polyVec.size(); i++)
        //polyVec[i].draw(buffer, colVec[i]);
     
     
     for(int i = 0; i < SCREEN_W; i += w)
       line(buffer, i, 0, i, SCREEN_H, makecol(64, 0, 0));
     for(int i = 0; i < SCREEN_H; i += h)
       line(buffer, 0, i, SCREEN_W, i, makecol(64, 0, 0));
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    
    
    printf("\n\n\n");
    FILE* file = fopen("output.txt", "w+");
    fprintf(file, "public class TileColliders {\n\
                      private var vec: Vector.<Poly>;\n\
                      private var w: int;\n\
                      private var h: int;\n\
	                     \n\
                      public function TileColliders()\n\
		                    {\n\
                        var p: Poly = null;\
			                     w = %d;\n\
	                       h = %d;\n", COLS, ROWS);
    
    
    for(int z = 0; z < 8; z++)
    for(int j = 0; j < ROWS; j++)
    for(int i = 0; i < COLS; i++)
    {  
       if(arr[i][j])
       {
        Poly poly = *arr[i][j];
        
        for(int k = 0; k < poly.vertexVec.size(); k++)
           poly.vertexVec[k] = transform(poly.vertexVec[k], z);
        poly.setVertexOrder(Poly::CW);
        
       fprintf(file, "p = new Poly();\n");
       for(int k = 0; k < poly.vertexVec.size(); k++)
       {
          fprintf(file, "p.vertexVec.push(new Vec2f(%f, %f));\n", poly.vertexVec[k].x,
                                                                  poly.vertexVec[k].y);
       
         if(z == DFLIP)
         {
            printf("%f %f,  ", poly.vertexVec[k].x, poly.vertexVec[k].y);
         }
       }
       
       if(z == DFLIP)
       printf("\n");
                                                                
       fprintf(file, "polyVec.push(p);\n\n");
       }
       
       else
       fprintf(file, "polyVec.push(null);\n\n");
    }
    
    fprintf(file, "}\n}\n");
    
    
    //getch();
    fclose(file);
    deinit();
    return 0;
}END_OF_MAIN()
