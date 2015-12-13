#ifndef _CLIPPER_HPP_
#define _CLIPPER_HPP_

#include <list>

#include "Vec2f.hpp"

struct Clipper {
      typedef std::list<Vec2f> PointList;
      typedef std::list<Vec2f>::iterator PointListIter;
      typedef std::list<PointList> StateList;
      
      PointList pointList;
      StateList stateList;
      
      static int whichSide(Vec2f const& c1, Vec2f const& c2, Vec2f const& p)
      {
        Vec2f n = (c1 - c2).unitNormal();
        float d1 = (p - c1).dot(n);
        
        if(d1 < 0)
          return -1;
        else
          return 1;
      }

      
      static bool clipSeg(Vec2f& p, Vec2f const& v1, Vec2f const& v2, Vec2f const& c1, Vec2f const& c2)
      {
             Vec2f n = (c1 - c2).normal();
             float d1 = (v1 - c1).dot(n);
             float d2 = (v2 - c1).dot(n);
             
             if(fabs(d1) < 0.001 && fabs(d2) < 0.001)
                printf("COPLANAR!");
  
             if((d1 < 0 && d2 >= 0) || (d2 < 0 && d1 >= 0))
             {
                    float k = fabs(d1) / (fabs(d1) + fabs(d2));
                    p = v1 + (v2 - v1) * k;
                    return true;
             }
  
             return false;
      }

      template <class T>
      static void circularErase(std::list<T>& list, typename std::list<T>::iterator i0, typename std::list<T>::iterator i1)
      {
           typename std::list<T>::iterator i;
           i0++;
     
           if(i0 == list.end())
              i0 = list.begin();
     
           for(i = i0; i != i1;)
           {
               i = list.erase(i);
        
               if(i == list.end())
                  i = list.begin();
           }
      }
      
      void pushState()
      {
           stateList.push_back(pointList);
      }
      
      void popState()
      {
           pointList = stateList.back();
           stateList.pop_back();
      }
      
      void pushPoint(Vec2f const& p)
      {
           pointList.push_back(p);
      }
      
      Vec2f popPoint()
      {
           Vec2f temp = pointList.front();
           
           if(!pointList.empty())
              pointList.pop_front();
           return temp;
      }
      
      Poly popPoly()
      {
           Poly poly(pointList.size());
           
           while(!isEmpty())
              poly.vertexVec.push_back(popPoint());
              
           return poly;
      }
      
      bool isEmpty()
      {
           return pointList.empty();
      }
      
      void clear()
      {
           pointList.clear();
      }
      
      void clip(Vec2f const& c1, Vec2f const& c2, int clipSide=-1)
      {
           Vec2f p[2];
           PointListIter ip[2];
           int ipNum = 0;
           
           for(PointListIter i = pointList.begin(); i != pointList.end(); ++i)
           {
               PointListIter i2 = i;
               ++i2;
               
               if(i2 == pointList.end())
                  i2 = pointList.begin();
               
               if(clipSeg(p[ipNum], *i, *i2, c1, c2))
               {
                  ip[ipNum] = i2;
                  ipNum++;
               }
           }
           
           if(ipNum == 2)
           {
             Vec2f temp = *ip[0];
             
             ip[0] = pointList.insert(ip[0], p[0]);
             ip[1] = pointList.insert(ip[1], p[1]);
             
             if(whichSide(c1, c2, temp) == clipSide) circularErase(pointList, ip[1], ip[0]);
             else                                    circularErase(pointList, ip[0], ip[1]);
           }
      }
};

#endif
