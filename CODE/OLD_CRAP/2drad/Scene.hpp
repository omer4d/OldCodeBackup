#ifndef _Scene_hpp_
#define _Scene_hpp_

#include <allegro.h>
#include <vector>
#include <float.h>

#include "Util.hpp"

#include "Vec2f.hpp"
#include "DrawVec2f.hpp"
#include "Col3f.hpp"

#include "Buffer.hpp"
#include "Line.hpp"
#include "Vertex.hpp"
#include "ScreenVertex.hpp"

#define LINE_MODE 0
#define POINT_MODE 1

#define SURFACE_LIGHTMAP_SIZE 32

struct Scene {
       Buffer<Col3f> colorBuffer;
       Buffer<float> depthBuffer;
       
       std::vector<Vertex> lineVertexVec;
       std::vector<Vertex> pointVertexVec;
       std::vector<Vertex>* vertexVec;
       
       std::vector<Buffer<Col3f>*> lightmapVec;
       
       Mat9f modelview, projection;
       Vec2f f0, f1, f2, f3, f4;
       Line clippers[4];
       Col3f lastCol;
       
       Vec2f camDisp;
       float camAng;
       
       Scene(int buffLen): colorBuffer(buffLen), depthBuffer(buffLen)
       {
        colorBuffer.clear(Col3f(0, 0, 0));
        depthBuffer.clear(1);
        
        lastCol.init(255, 255, 255);
        
        camDisp.init(0, 0);
        camAng = 0;
        
        vertexVec = &lineVertexVec;
       }
       
       ~Scene()
       {
        for(int i = 0; i < lightmapVec.size(); i++)
         delete lightmapVec[i];
       }
       
       void setMode(int mode)
       {
        if(mode == LINE_MODE)
         vertexVec = &lineVertexVec;
        else if(mode == POINT_MODE)
         vertexVec = &pointVertexVec;
       }
       
       void initProjection(float b, float t, float n, float f)
       {
        Mat9f proj;
        
        proj(0, 0) = 1 / (f - n); proj(1, 0) = 0;     proj(2, 0) = -n / (f - n);
        proj(0, 1) = -b / (t - b); proj(1, 1) = n / (t - b);  proj(2, 1) = 0; 
        proj(0, 2) = 1; proj(1, 2) = 0;     proj(2, 2) = 0;
        
        this->projection = proj;
        
        f0.init(0, 0);
        f1.init(n, t);
        f4.init(n, b);
        f2 = (f1 - f0) * (f / n);
        f3 = (f4 - f0) * (f / n);
    
        clippers[0] = Line(f1, f2);
        clippers[1] = Line(f2, f3);
        clippers[2] = Line(f3, f4);
        clippers[3] = Line(f4, f1);
       }
       
       void pushVertex(Vec2f const& v)
       {
        Vertex vert;
        vert.pos = v;
        vert.col = lastCol;
        vertexVec->push_back(vert);
       }
       
       void pushPoly(Vec2f arr[], int n)
       {
         int i;
        
         for(i = 0; i < n - 1; i++)
         {
          pushVertex(arr[i]);
          pushVertex(arr[i + 1]);
         }
         
         pushVertex(arr[i]);
         pushVertex(arr[0]);
       }
       
       void pushColor(Col3f const& c)
       {
        lastCol = c;
       }
       
       void uploadFinished()
       {
        for(int i = 0; i < lineVertexVec.size() / 2; i++)
        {
         Buffer<Col3f>* lm = new Buffer<Col3f>(SURFACE_LIGHTMAP_SIZE);
         lm->clear(Col3f(0, 0, 0));
         lightmapVec.push_back(lm);
        }
       }
       
       // *************
       // * RENDERING *
       // *************
       
       void renderLine(ScreenVertex const& a, ScreenVertex const& b)
       {
        ScreenVertex const& min = a.y < b.y ? a : b;
        ScreenVertex const& max = a.y > b.y ? a : b;
        int miny = int(min.y + 1), maxy = int(max.y);
        ScreenVertex accum = min, delta = ScreenVertex::calcDelta(min, max, maxy - miny);
        float depth;
        
        if(miny < maxy)
        {
          for(int i = miny; i <= maxy; i++)
            {
                  depth = 1 / accum.invDepth;
           
                  if(depth < depthBuffer.data[i])
                  {
                    colorBuffer.data[i] = accum.col;
                    depthBuffer.data[i] = depth;
                  }
            
                  accum += delta;
            }
          }
       }
       
       void renderLine(ScreenVertex const& a, ScreenVertex const& b, Buffer<Col3f> const& lightmap)
       {
        ScreenVertex const& min = a.y < b.y ? a : b;
        ScreenVertex const& max = a.y > b.y ? a : b;
        int miny = int(min.y + 1), maxy = int(max.y);
        ScreenVertex accum = min, delta = ScreenVertex::calcDelta(min, max, maxy - miny);
        float x0 = 1 / min.invDepth, x1 = 1 / max.invDepth, depth, k;
        
        if(miny < maxy)
        {
         if(fabs(x1 - x0) > 0.0001)
         {
          float t = 1 / (x1 - x0);
          for(int i = miny; i <= maxy; i++)
            {
                  depth = 1 / accum.invDepth;
                  k = (depth - x0) * t;
           
                  if(k < 0.0) k = 0.0;
                  if(k > 1.0) k = 1.0;
           
                  if(depth < depthBuffer.data[i])
                  {
                    colorBuffer.data[i] = lightmap.data[int(k * (lightmap.len - 1))] * accum.col;
                    depthBuffer.data[i] = depth;
                  }
            
                  accum += delta;
            }
          }
          
          else
          {
          float t = 1.0 / (maxy - miny);
          for(int i = miny; i <= maxy; i++)
            {
                  depth = 1 / accum.invDepth;
                  k = (i - miny) * t;
           
                  if(k < 0.0) k = 0.0;
                  if(k > 1.0) k = 1.0;
           
                  if(depth < depthBuffer.data[i])
                  {
                    colorBuffer.data[i] = lightmap.data[int(k * (lightmap.len - 1))] * accum.col;
                    depthBuffer.data[i] = depth;
                  }
            
                  accum += delta;
            }
          }
         }
       }
       
       void render()
       {
        Vertex v1, v2;
        colorBuffer.clear(Col3f(0, 0, 0));
        depthBuffer.clear(1);
        
        modelview.initIdentity();
        modelview.rotate(-camAng);
        modelview.translate(-camDisp.x, -camDisp.y);
       
        for(int i = 0; i < lineVertexVec.size() / 2; i++)
        {
          v1 = lineVertexVec[i * 2];
          v2 = lineVertexVec[i * 2 + 1];
         
          v1.pos *= modelview;
          v2.pos *= modelview;
         
          bool vis;
          for(int j = 0; j < 4 && (vis = Vertex::clip(v1, v2, clippers[j])); j++);
         
          if(vis)
          {
             v1.pos *= projection;
             v2.pos *= projection;
             renderLine(ScreenVertex(v1, colorBuffer.len), ScreenVertex(v2, colorBuffer.len), *lightmapVec[i]);
          }
        }

        for(int i = 0; i < pointVertexVec.size(); i++)
        {
          Vec2f c = pointVertexVec[i].pos;
          v1 = pointVertexVec[i];
          v2 = pointVertexVec[i];

          c *= modelview;
          
          Vec2f delta = -c;
          float r = 30.0;
          float h = delta.length();
          float cosa = r / h;
          float alpha = acos(cosa);
          
          if(cosa <= 1.0)
          {
          v1.pos = c + (delta * cosa).rotated(alpha);
          v2.pos = c + (delta * cosa).rotated(-alpha);
          }
          
          else
          {
           v1.pos = f1;
           v2.pos = f4;
          }
         
         
          bool vis;
          for(int j = 0; j < 4 && (vis = Vertex::clip(v1, v2, clippers[j])); j++);
         
          if(vis)
          {
             v1.pos *= projection;
             v2.pos *= projection;
             renderLine(ScreenVertex(v1, colorBuffer.len), ScreenVertex(v2, colorBuffer.len));
          }
        }
       }
       
       // ***************
       // * RANDOM CRAP *
       // ***************
       
       Col3f colorBufferSum(Buffer<float> const& weights)
       {
         Col3f s(0, 0, 0);
         
         for(int i = 0; i < colorBuffer.len; i++)
           s += colorBuffer.data[i] * weights.data[i];
           
         return s;
       }
       
       void output(BITMAP* buffer)
       {
        int w = 100, h = 2;
        
        for(int i = 0; i < colorBuffer.len; i++)
        {
         Col3f& c = colorBuffer.data[i];
         rectfill(buffer, 0, i * h, w, (i + 1) * h, makecol(clamp255(c.r), clamp255(c.g), clamp255(c.b)));
        }
       }
       
       void debugRender(BITMAP* buffer)
       {
        Mat9f cam;
        cam.initIdentity();
        cam.translate(camDisp.x, camDisp.y);
        cam.rotate(camAng);
        
        Vec2f tf0 = f0 * cam;
        Vec2f tf1 = f1 * cam;
        Vec2f tf2 = f2 * cam;
        Vec2f tf3 = f3 * cam;
        Vec2f tf4 = f4 * cam;
        
       for(int i = 0; i < lineVertexVec.size() / 2; i++)
             DrawVec2f::line(buffer, lineVertexVec[i * 2].pos, lineVertexVec[i * 2 + 1].pos, makecol(255, 0, 0));
             
       for(int i = 0; i < pointVertexVec.size(); i++)
             DrawVec2f::circle(buffer, pointVertexVec[i].pos, 10, makecol(255, 0, 0));
       
       DrawVec2f::line(buffer, tf1, tf2, makecol(0, 255, 0));
       DrawVec2f::line(buffer, tf2, tf3, makecol(0, 255, 0));
       DrawVec2f::line(buffer, tf3, tf4, makecol(0, 255, 0));
       DrawVec2f::line(buffer, tf4, tf1, makecol(0, 255, 0));
     
       DrawVec2f::line(buffer, tf0, tf1, makecol(0, 255, 0));
       DrawVec2f::line(buffer, tf0, tf4, makecol(0, 255, 0));
       }
};

#endif
