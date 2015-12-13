#ifndef _RAYTRACER_HPP_
#define _RAYTRACER_HPP_

#include <allegro.h>

typedef std::function<bool(int, int, int)> RayTracerCallback;
typedef std::function<int(int)> GetTypeColor;

static BITMAP* G_RAYTRACER_BUFFER = nullptr;

void setRayTracerBuffer(BITMAP* bmp)
{
	G_RAYTRACER_BUFFER = bmp;
}

struct HitInfo {
    bool hit;
    int parkI, parkJ;
    int hitI, hitJ, hitType;
    
    HitInfo()
    {
       this->hit = false;
       this->parkI = -1;
       this->parkJ = -1;
    }
    
    HitInfo(int hitI, int hitJ, int hitType, int parkI, int parkJ)
    {
        this->hit = true;
        this->parkI = parkI;
        this->parkJ = parkJ;
        this->hitI = hitI;
        this->hitJ = hitJ;
        this->hitType = hitType;
    }
};

float rayVsCircle(Vec2f const& src, Vec2f const& dir, Vec2f const& center, float rad)
{
    float a = dir.dot(dir);
    float b = 2 * (src - center).dot(dir);
    float c = (src - center).dot(src - center) - rad * rad;
    float d = b*b - 4.f * a * c;
    
    if(d >= 0.f)
    {
        d = sqrt(d);
        float t1 = (-b + d) / (2.f * a);
        float t2 = (-b - d) / (2.f * a);
        return MIN(t1, t2);
    }
    
    return -1.f;
}

int sgni(float f)
{
   return f < 0 ? -1 : 1;
}

HitInfo raytrace(GridMatrix<int>& grid, Vec2f orig, Vec2f dir, int refs = 1, RayTracerCallback const& callback=nullptr, bool vis=false, GetTypeColor getTypeColor=nullptr)
{
     float cellW = grid.cellSize * 0.5f;
     float cellH = grid.cellSize * (float)sqrt(3.f) * 0.5f;
     
     Vec2f realOrig = orig, realDir = dir;
     
     dir.y *= (cellW / cellH);
     orig.x /= cellW;
     orig.y /= cellH;
     
     int x = int(orig.x), y = int(orig.y);
     int stepX = sgni(dir.x), stepY = sgni(dir.y);    
     
     float tMaxX = (int(orig.x + (stepX > 0)) - orig.x) / dir.x;
     float tMaxY = (int(orig.y + (stepY > 0)) - orig.y) / dir.y;
     float tDeltaX = float(stepX) / dir.x;
     float tDeltaY = float(stepY) / dir.y;
     float t = 0.0;
     
     HitInfo hitInfo;
     int lastI = (x - (y % 2)) / 2, lastJ = y;
     
     while(y >= 0 && y < grid.rowNum)
     {
        int i = (x - (y % 2)) / 2;
        int cellContent = grid.at(i, y);

        if((i != lastI || y != lastJ) && cellContent != 0) // Collision?
        {
            //Vec2f center(grid.toWx(i, y), grid.toWy(y));          
            //if(rayVsCircle(realOrig, realDir, center, CELL_SIZE / 2) > 0.f)
            
            if(!callback || callback(i, y, cellContent))
            {
            	hitInfo = HitInfo(i, y, cellContent, lastI, lastJ);
            	break;
            }
        }
        
        if(tMaxX < tMaxY) // Horizontal movement?
        {
           lastI = i;
           lastJ = y;
         
           t = tMaxX;
           tMaxX += tDeltaX;
           x += stepX;
           
           if((x == 0 && stepX < 0) || (x == grid.colNum * 2 - 1 && stepX > 0)) // Reflection?
           {
              if(refs > 0)
                hitInfo = raytrace(grid, realOrig + realDir * (t * 0.999f) * cellW, 
                                         Vec2f(-realDir.x, realDir.y), refs - 1, callback, vis, getTypeColor);
              break;
           }
        }
        
        else // Vertical movement?
        {
            lastI = i;
            lastJ = y;
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
            
            if(y < 0)
            {
            	hitInfo = HitInfo(i, y, 0, lastI, lastJ);
            	break;
            }
        }
     }
     
     if(vis && getTypeColor && G_RAYTRACER_BUFFER)
     {
        //int cols[TYPENUM] = {makecol(255, 0, 0), makecol(0, 255, 0), makecol(0, 0, 255),
        //                     makecol(255, 255, 0), makecol(0, 255, 255), makecol(255, 0, 255),
        //                     makecol(255, 255, 255)};
     
        if(hitInfo.hit)
        {
        	if(hitInfo.hitJ < 0)
        		DrawVec2f::line(G_RAYTRACER_BUFFER, realOrig, realOrig + realDir * t * cellW, makecol(128, 128, 128));
        	else
           		DrawVec2f::line(G_RAYTRACER_BUFFER, realOrig, realOrig + realDir * t * cellW,
                                getTypeColor(grid.at(hitInfo.hitI, hitInfo.hitJ)));
        }
        else
           DrawVec2f::line(G_RAYTRACER_BUFFER, realOrig, realOrig + realDir * t * cellW, makecol(128, 128, 128));
     }
     
     return hitInfo;
}


//HitInfo shootDebugRay(GridMatrix<int>& grid, Vec2f const& src, float range, int refs, int res, int n)
//{
//   Vec2f dir = Vec2f(0.f, -1.f).rotated(-range / 2 + range / (res + 1) * n);
//   HitInfo info = raytrace(grid, src, dir, refs, nullptr, true);
//        
//   if(info.hit)
//   {
//       circlefill(buffer, int(grid.toWx(info.parkI, info.parkJ)),
//                          int(grid.toWy(info.parkJ)), 5, makecol(255, 255, 255));
//                             
//       circlefill(buffer, int(grid.toWx(info.hitI, info.hitJ)),
//                          int(grid.toWy(info.hitJ)), 5, 0);
//   }
//   
//   return info;
//}

#endif
