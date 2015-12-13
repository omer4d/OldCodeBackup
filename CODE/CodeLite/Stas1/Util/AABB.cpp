#include "AABB.hpp"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void AABB::setCorners(Vec2f const& p0, Vec2f const& p1)
{
    min.x = p0.x < p1.x ? p0.x : p1.x;
    min.y = p0.y < p1.y ? p0.y : p1.y;
    max.x = p0.x > p1.x ? p0.x : p1.x;
    max.y = p0.y > p1.y ? p0.y : p1.y;
}

AABB::AABB(void)
{
    setCorners(Vec2f(), Vec2f());
}

AABB::AABB(float x1, float y1, float x2, float y2)
{
    min.x = MIN(x1, x2);
    min.y = MIN(y1, y2);
    max.x = MAX(x1, x2);
    max.y = MAX(y1, y2);
}

AABB::AABB(Vec2f const& p0, Vec2f const& p1)
{
    setCorners(p0, p1);
}

Vec2f AABB::getPoint(float px, float py) const
{
    return Vec2f(min.x + width() * px, min.y + height() * py);
}

Vec2f AABB::getMin(void) const
{
    return min;
}

Vec2f AABB::getMax(void) const
{
    return max;
}

float AABB::width(void) const
{
    return max.x - min.x;
}

float AABB::height(void) const
{
    return max.y - min.y;
}

AABB AABB::subAABB(float px0, float py0, float px1, float py1) const
{
    return AABB(Vec2f(min.x + (max.x - min.x) * px0, min.y + (max.y - min.y) * py0),
            Vec2f(min.x + (max.x - min.x) * px1, min.y + (max.y - min.y) * py1));
}

void AABB::setCornerWH(Vec2f const& p, float w, float h)
{
    setCorners(p, Vec2f(p.x + w, p.y + h));
}

void AABB::setCenterWH(Vec2f const& c, float w, float h)
{
    w *= 0.5;
    h *= 0.5;
    setCorners(Vec2f(c.x - w, c.y - h), Vec2f(c.x + w, c.y + h));
}

void AABB::translate(float tx, float ty)
{
    min.x += tx;
    min.y += ty;
    max.x += tx;
    max.y += ty;
}

bool AABB::contains(Vec2f const& v) const
{
    if(v.x < min.x || v.x > max.x || v.y < min.y || v.y > max.y) return false;
    return true;
}

bool AABB::overlaps(AABB const& other) const
{
    if(min.x > other.max.x || min.y > other.max.y || other.min.x > max.x || other.min.y > max.y) return false;
    return true;
}

AABB AABB::bound(AABB const& a, AABB const& b)
{
    AABB res;
    
    res.min.x = MAX(a.min.x, b.min.x);
    res.max.x = MIN(a.max.x, b.max.x);
    
    res.min.y = MAX(a.min.y, b.min.y);
    res.max.y = MIN(a.max.y, b.max.y);
    
    return res;
}