#ifndef _AABB_h_
#define _AABB_h_

#include "Vec2f.hpp"

struct AABB {
    Vec2f min, max;
    
    void setCorners(Vec2f const& p0, Vec2f const& p1);
    AABB(void);
    AABB(Vec2f const& p0, Vec2f const& p1);
    AABB(float x1, float y1, float x2, float y2);

    Vec2f getPoint(float px, float py) const;
    Vec2f getMin(void) const;
    Vec2f getMax(void) const;
    float width(void) const;
    float height(void) const;
    
    AABB subAABB(float px0, float py0, float px1, float py1) const;
    void setCornerWH(Vec2f const& p, float w, float h);
    void setCenterWH(Vec2f const& c, float w, float h);
    
    void translate(float tx, float ty);

    bool contains(Vec2f const& v) const;
    bool overlaps(AABB const& other) const;
    
    static AABB bound(AABB const& a, AABB const& b);
};

#endif
