/* 
 * File:   Poly.hpp
 * Author: Stas
 *
 * Created on June 24, 2012, 10:03 PM
 */

#ifndef POLY_HPP
#define	POLY_HPP

#include <vector>
#include <algorithm>
#include <float.h>

#include "Vec2f.hpp"
#include "Shape.hpp"
#include "Interval.hpp"
#include "SAT.hpp"
#include "Ray.hpp"

#include <float.h>

struct Poly : public Shape {
    enum VertexOrder {
        CW, CCW
    };
    
    std::vector<Vec2f> vertexVec;

    Poly()
    {
        
    }
    
    Poly(int n)
    {
        vertexVec.reserve(n);
    }
    
    int vertexNum() const
    {
        return vertexVec.size();
    }
    
    void initRect(float x1, float y1, float x2, float y2)
    {
        vertexVec.clear();
        vertexVec.reserve(4);
        vertexVec.push_back(Vec2f(x1, y1));
        vertexVec.push_back(Vec2f(x2, y1));
        vertexVec.push_back(Vec2f(x2, y2));
        vertexVec.push_back(Vec2f(x1, y2));
    }

    void init(int n, float rx, float ry, float ang = 0.f)
    {
        vertexVec.clear();
        vertexVec.reserve(n);

        if(n % 2) ang += M_PI / (n * 2.f);
        else ang += M_PI / n;

        for(int i = 0; i < n; i++)
        {
            float k = float(i) / float(n);
            float x = rx * cos(k * 2.f * M_PI + ang);
            float y = ry * sin(k * 2.f * M_PI + ang);
            vertexVec.push_back(Vec2f(x, y));
        }
    }

    void initRegular(int n, float r, float ang = 0.f)
    {
        init(n, r, r, ang);
    }

    void initStar(int n, float r1, float r2, float ang = 0.f)
    {
        vertexVec.clear();
        vertexVec.reserve(n * 2);

        if(n % 2) ang += M_PI / (n * 2.f);
        else ang += M_PI / n;

        for(int i = 0; i < n * 2; i++)
        {
            float k = float(i) / float(n * 2);
            float r = i % 2 ? r1 : r2;
            float x = r * cos(k * 2.f * M_PI + ang);
            float y = r * sin(k * 2.f * M_PI + ang);
            vertexVec.push_back(Vec2f(x, y));
        }
    }

    bool contains(Vec2f const& p, Mat9f const& mat=Mat9f::identity) const
    {
        int cn = 0;
        int n = vertexVec.size();

        for(int i = 0; i < n; i++)
        {
            Vec2f v0 = vertexVec[i] * mat;
            Vec2f v1 = vertexVec[(i + 1) % n] * mat;

            if((v0.y <= p.y && v1.y > p.y) || (v0.y > p.y) && (v1.y <= p.y))
            {
                float vt = (p.y - v0.y) / (v1.y - v0.y);

                if(p.x < v0.x + vt * (v1.x - v0.x))
                    cn++;
            }
        }

        return cn % 2;
    }

    Vec2f getNormal(int i) const
    {
        return (vertexVec[(i + 1) % vertexVec.size()] - vertexVec[i]).normal();
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
    
    void transform(Mat9f const& mat)
    {
        for(int i = 0; i < vertexVec.size(); i++)
            vertexVec[i] *= mat;
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
    
    Interval project(Mat9f const& mat, Vec2f const& v) const
    {
        Interval interval(FLT_MAX, -FLT_MAX);

        for(int i = 0; i < vertexVec.size(); i++)
        {
            float k = (vertexVec[i] * mat).dot(v);
            
            if(k < interval.min)
                interval.min = k;

            if(k > interval.max)
                interval.max = k;
        }

        return interval;
    }
    
    void pushAxisNormals(Mat9f const& mat, bool normalize) const
    {
        SAT& sat = SAT::getInstance();
        std::vector<Vec2f> const& p1 = vertexVec;
        int i;
        
        if(normalize)
        {
            for(i = 0; i < p1.size() - 1; i++)
                sat.pushAxisNormal((p1[i] - p1[i + 1]).unitNormal());
            sat.pushAxisNormal((p1[i] - p1[0]).unitNormal());

        }
        
        else
        {
            for(i = 0; i < p1.size() - 1; i++)
                sat.pushAxisNormal((p1[i] - p1[i + 1]).normal());
            sat.pushAxisNormal((p1[i] - p1[0]).normal());
        }
    }
    
    static bool overlap(Poly const& poly1, Mat9f const& mat1,
                        Poly const& poly2, Mat9f const& mat2)
    {
        SAT& sat = SAT::getInstance();

        sat.clearAxisNormals();
        
        poly1.pushAxisNormals(mat1, false);
        poly2.pushAxisNormals(mat2, false);
        
        return sat.test(poly1, mat1, poly2, mat2);
    }
    
    static bool overlap(Poly const& poly1, Mat9f const& mat1,
                        Poly const& poly2, Mat9f const& mat2, Vec2f &mtd, Vec2f& mtdNorm)
    {
        SAT& sat = SAT::getInstance();
        sat.clearAxisNormals();
        
        poly1.pushAxisNormals(mat1, true);
        poly2.pushAxisNormals(mat2, true);

        return sat.test(poly1, mat1, poly2, mat2, mtd, mtdNorm);
    }

    float intersect(Vec2f& norm, Mat9f const& mat, Ray const& ray) const
    {
        int n = vertexVec.size(), counter = 0;
        float dist, minDist = FLT_MAX;

        for(int i = 0; i < vertexVec.size(); i++)
        {
            if(ray.intersectLine(dist, vertexVec[i] * mat, vertexVec[(i + 1) % n] * mat))
            {
                counter++;

                if(dist < minDist)
                {
                    norm = (vertexVec[(i + 1) % n] - vertexVec[i]).normal();
                    minDist = dist;
                }
            }
        }

        if(counter < 2)
            return FLT_MAX;
        
        return minDist;
    }
    
    Vec2f average() const
    {
        Vec2f sum;
        
        for(int i = 0; i < vertexVec.size(); i++)
            sum += vertexVec[i];
        
        sum *= (1.0 / vertexVec.size());
        return sum;
    }
};

#endif	/* POLY_HPP */

