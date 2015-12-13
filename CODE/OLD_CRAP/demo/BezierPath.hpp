/* 
 * File:   BezierPath.hpp
 * Author: Stas
 *
 * Created on January 25, 2012, 2:32 AM
 */

#ifndef BEZIERPATH_HPP
#define	BEZIERPATH_HPP

#include <list>
#include <vector>

#include "Vec2f.hpp"

struct BezierPathNode {
    Vec2f p, g1, g2;
    float d, len;
    
    BezierPathNode(Vec2f const& p)
    {
        this->p = p;
    }
};

class BezierPath {
    std::vector<BezierPathNode> nodeVec;
    int realNodeNum;

    static Vec2f cubicBezier(Vec2f const& p0, Vec2f const& p1, Vec2f const& p2, Vec2f const& p3, float t)
    {
        float k = (1 - t), k2 = k * k, t2 = t*t;
        return p0 * (k2 * k) + p1 * (3 * t * k2) + p2 * (3 * t2 * k) + p3 * (t2 * t);
    }
    
    static Vec2f cubicBezierTangent(Vec2f const& p0, Vec2f const& p1, Vec2f const& p2, Vec2f const& p3, float t)
    {
     return ((p3 - p0) * 3.0 + (p1 - p2) * 9.0)*t*t + (p0 + p2 - p1*2.0)*6.0*t + (p1-p0)*3.0;
    }
    
    static float cubicBezierLen(Vec2f const& p0, Vec2f const& p1, Vec2f const& p2, Vec2f const& p3)
    {
        Vec2f last = p0, curr;
        float len = 0.0;
        
        for(int i = 1; i < 10; i++)
        {
            curr = cubicBezier(p0, p1, p2, p3, i / 9.0);
            len += (curr - last).length();
            last = curr;
        }
        
        return len;
    }
    
public:
    
    BezierPath(std::list<Vec2f> const& path, double curviness)
    {
        realNodeNum = path.size();
        int& n = realNodeNum;
        
        // Create the nodes, including two dummy nodes at the start and end:
        nodeVec.reserve(n + 2);
        nodeVec.push_back(BezierPathNode(Vec2f()));
        
        for(std::list<Vec2f>::const_reverse_iterator i = path.rbegin(); i != path.rend(); i++)
            nodeVec.push_back(BezierPathNode(*i));
        
        nodeVec.push_back(BezierPathNode(Vec2f()));

        nodeVec[0].p = nodeVec[1].p + (nodeVec[1].p - nodeVec[2].p) * 0.25;
        nodeVec[n + 1].p = nodeVec[n].p + (nodeVec[n].p - nodeVec[n - 1].p) * 0.25;
        
        // For each node, compute the distance to the next one:
        for(int i = 0; i < n + 1; i++)
            nodeVec[i].d = (nodeVec[i].p - nodeVec[i + 1].p).length();
        
        // Compute the guide points for the real nodes:
        for(int i = 1; i < n + 1; i++)
        {
            Vec2f t = (nodeVec[i + 1].p - nodeVec[i - 1].p).unit();
            
            nodeVec[i].g1 = nodeVec[i]. p - t * nodeVec[i - 1].d * curviness;
            nodeVec[i].g2 = nodeVec[i].p + t * nodeVec[i].d * curviness;
        }
        
        // Compute each nodes's distance along the path, __including the last dummy node__.
        nodeVec[1].len = 0.0;
        for(int i = 1; i < realNodeNum; i++)
            nodeVec[i + 1].len = nodeVec[i].len + cubicBezierLen(nodeVec[i].p, nodeVec[i].g2, nodeVec[i + 1].g1, nodeVec[i + 1].p);      
    }
    
    float len() const
    {
        return nodeVec[realNodeNum].len;
    }
    
    Vec2f point(float s) const
    {
        if(s < 0.0) s = 0.0;
        if(s > nodeVec[realNodeNum].len) s = nodeVec[realNodeNum].len;
        
        int i;
        for(i = 1; i < realNodeNum && nodeVec[i].len < s; i++);
        
        float a = 1.0 - (nodeVec[i].len - s) / (nodeVec[i].len - nodeVec[i - 1].len);
        return cubicBezier(nodeVec[i - 1].p, nodeVec[i - 1].g2, nodeVec[i].g1, nodeVec[i].p, a);
    }
    
    Vec2f tangent(float s) const
    {
        if(s < 0.0) s = 0.0;
        if(s > nodeVec[realNodeNum].len) s = nodeVec[realNodeNum].len;
        
        int i;
        for(i = 1; i < realNodeNum && nodeVec[i].len < s; i++);
        
        float a = 1.0 - (nodeVec[i].len - s) / (nodeVec[i].len - nodeVec[i - 1].len);
        return cubicBezierTangent(nodeVec[i - 1].p, nodeVec[i - 1].g2, nodeVec[i].g1, nodeVec[i].p, a);
    }
    
    void render(BITMAP* buffer, float tx, float ty, float scale, int col) const
    {
        int points[8];

        for(int i = 1; i < realNodeNum; i++)
        {
            points[0] = (int)(tx + nodeVec[i].p.x * scale);
            points[1] = (int)(ty + nodeVec[i].p.y * scale);

            points[2] = (int)(tx + nodeVec[i].g2.x * scale);
            points[3] = (int)(ty + nodeVec[i].g2.y * scale);

            points[4] = (int)(tx + nodeVec[i + 1].g1.x * scale);
            points[5] = (int)(ty + nodeVec[i + 1].g1.y * scale);

            points[6] = (int)(tx + nodeVec[i + 1].p.x * scale);
            points[7] = (int)(ty + nodeVec[i + 1].p.y * scale);
            
            spline(buffer, points, col);
            //drawCubicBezier(buffer, nodeVec[i].p, nodeVec[i].g2, nodeVec[i + 1].g1, nodeVec[i + 1].p, makecol(0, 0, 255));
        }
    }
    
    void testRender(BITMAP* buffer, int col) const
    {
        Vec2f v = point(0.0);
        int lastX = (int)v.x, lastY = (int)v.y;

        for(int i = 1; i < 100; i++)
        {
            v = point(i / 99.0);
            int x = (int) v.x, y = (int) v.y;
            Vec2f t = v + tangent(i / 99.0).unit() * 100.0;

            line(buffer, x, y, (int)t.x, (int)t.y, makecol(255, 0, 0));
            line(buffer, lastX, lastY, x, y, col);

            lastX = x;
            lastY = y;
        }
    }
};

#endif	/* BEZIERPATH_HPP */

