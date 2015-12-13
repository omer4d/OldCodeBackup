#include "DrawVec2f.hpp"

float DrawVec2f::tx = 0;
float DrawVec2f::ty = 0;
float DrawVec2f::sx = 1;
float DrawVec2f::sy = 1;
float DrawVec2f::su = 1;

void DrawVec2f::init(float tx, float ty, float sx, float sy, float su)
{
    DrawVec2f::tx = tx;
    DrawVec2f::ty = ty;
    DrawVec2f::sx = sx;
    DrawVec2f::sy = sy;
    DrawVec2f::su = su;
}

int DrawVec2f::scrx(float x)
{
    return int(x * sx + tx);
}

int DrawVec2f::scry(float y)
{
    return int(y * sy + ty);
}

void DrawVec2f::line(BITMAP* buffer, Vec2f const& a, Vec2f const& b, int col)
{
    ::line(buffer, scrx(a.x), scry(a.y),
           scrx(b.x), scry(b.y), col);
}

void DrawVec2f::pixel(BITMAP* buffer, Vec2f const& p, int col)
{
    ::putpixel(buffer, scrx(p.x), scry(p.y), col);
}

void DrawVec2f::circle(BITMAP* buffer, Vec2f const& p, float r, int col)
{
    ::circle(buffer, scrx(p.x), scry(p.y), int(r * su), col);
}

void DrawVec2f::circlefill(BITMAP* buffer, Vec2f const& p, int r, int col)
{
    ::circlefill(buffer, scrx(p.x), scry(p.y), int(r * su), col);
}

void DrawVec2f::rect(BITMAP* buffer, Vec2f const& p1, Vec2f const& p2, int col)
{
    ::rect(buffer, scrx(p1.x), scry(p1.y), scrx(p2.x), scry(p2.y), col);
}

void DrawVec2f::rect(BITMAP* buffer, Vec2f const& p1, float w, float h, int col)
{
    ::rect(buffer, scrx(p1.x), scry(p1.y), scrx(p1.x + w), scry(p1.y + h), col);
}

void DrawVec2f::rectfill(BITMAP* buffer, Vec2f const& p1, Vec2f const& p2, int col)
{
    ::rectfill(buffer, scrx(p1.x), scry(p1.y), scrx(p2.x), scry(p2.y), col);
}

void DrawVec2f::rectfill(BITMAP* buffer, Vec2f const& p1, float w, float h, int col)
{
    ::rectfill(buffer, scrx(p1.x), scry(p1.y), scrx(p1.x + w), scry(p1.y + h), col);
}

void DrawVec2f::poly(BITMAP* buffer, Poly const& poly, int fillCol, int lineCol,
                            Mat9f const& mat)
{
    if(fillCol != -1)
    {
        int* points = new int[poly.vertexVec.size() * 2];

        for(int i = 0; i < poly.vertexVec.size(); i++)
        {
            Vec2f t = poly.vertexVec[i] * mat;
            points[i * 2] = int(scrx(t.x));
            points[i * 2 + 1] = int(scry(t.y));
        }

        ::polygon(buffer, poly.vertexVec.size(), points, fillCol);
        delete[] points;
    }

    if(lineCol != -1)
    {
        int i;
        for(i = 0; i < poly.vertexVec.size() - 1; i++)
        {
            line(buffer, poly.vertexVec[i] * mat, poly.vertexVec[i + 1] * mat, lineCol);
        }

        line(buffer, poly.vertexVec[i] * mat, poly.vertexVec[0] * mat, lineCol);
    }

}