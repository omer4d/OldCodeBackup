/* 
 * File:   Poly.hpp
 * Author: Stas
 *
 * Created on June 24, 2012, 10:03 PM
 */

#ifndef POLY_HPP
#define	POLY_HPP

#include <vector>
#include <float.h>

#include "Vec2f.hpp"

class Ray;

struct Poly {
    std::vector<Vec2f> vertexVec;

    void init(float px, float py, float r, int vnum)
    {
        vertexVec.clear();
        vertexVec.reserve(vnum);

        for(int i = 0; i < vnum; i++)
        {
            float alpha = float(i) / float(vnum) * 2 * M_PI;
            Vec2f v(px + r * cos(alpha), py + r * sin(alpha));
            vertexVec.push_back(v);
        }
    }
    
        float intersect(Vec2f& norm, Ray const& ray) const;
        
     void draw(BITMAP* buffer, int col, int lineCol=-1)
  {
   
     if(col != -1)
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
     
     if(lineCol != -1)
     {
      int i;
     for(i = 0; i < vertexVec.size() - 1; i++)
     {
         line(buffer, int(vertexVec[i].x + 0.5f), int(vertexVec[i].y + 0.5f),
                      int(vertexVec[i + 1].x + 0.5f), int(vertexVec[i + 1].y + 0.5f), lineCol);
     }
     
              line(buffer, int(vertexVec[i].x + 0.5f), int(vertexVec[i].y + 0.5f),
                      int(vertexVec[0].x + 0.5f), int(vertexVec[0].y + 0.5f), lineCol);
     }
     
  }
};

#endif	/* POLY_HPP */

