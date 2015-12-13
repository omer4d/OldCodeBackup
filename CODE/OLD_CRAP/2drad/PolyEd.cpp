
#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <fstream>

#include <allegro.h>
#include <alpng/alpng.h>
#include <list>
#include <algorithm>

#include "Keyboard.hpp"
#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

#include <vector>

#include <json/json.h>

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

#define SNAP_RAD 5

void init()
{
    allegro_init();

    install_mouse();
    install_keyboard();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);

    buffer = create_bitmap(SCREEN_W, SCREEN_H);

    srand(time(NULL));
    
     set_alpha_blender();
     //set_difference_blender(255, 255, 255, 255);
     
     alpng_init();
}

void alphaOn()
{
	drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
}

void alphaOff()
{
	drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
}

void deinit()
{
    destroy_bitmap(buffer);
}

Vec2f genPoint(Vec2f const& a, Vec2f const& b, Vec2f const& c, float w)
{
    Vec2f v1 = a - b, v2 = c - b;
    Vec2f n1 = (b - a).unitNormal() * w, n2 = (c - b).unitNormal() * w;
    Vec2f dn = n2 - n1;
    float k2 = dn.cross(v1) / v1.cross(v2);
    return b + v2 * k2 + n2;
}

struct Poly {
    std::vector<Vec2f> pointVec;
};

void drawPoly(BITMAP* buffer, Poly const& poly, int c)
{
	alphaOn();
	
               int* points = new int[poly.pointVec.size() * 2];
     
               for(int i = 0; i < poly.pointVec.size(); i++)
               {
                       points[i * 2]     = int(poly.pointVec[i].x);
                       points[i * 2 + 1] = int(poly.pointVec[i].y);
               }
     
               ::polygon(buffer, poly.pointVec.size(), points, c);
               delete[] points;
               
	alphaOff();
}

float polygonArea(std::vector<Vec2f> const& poly)
{
    float s = 0;

    for(int i = 0; i < poly.size(); i++)
    {
        int j = (i + 1) % poly.size();
        s += poly[i].x * poly[j].y - poly[j].x * poly[i].y;
    }

    s *= 0.5;

    return s;
}

enum Side {
	POS_SIDE, NEG_SIDE, EITHER_SIDE
};

Side whichSide(Vec2f a, Vec2f b, Vec2f c)
{
	Vec2f n = (b - a).normal();
	float dp = n.dot(c - a);
	
	if(dp < 0)
		return NEG_SIDE;
	else if(dp > 0)
		return POS_SIDE;
	else
		return EITHER_SIDE;
}

bool sameSide(Side s1, Side s2)
{
	return s1 == EITHER_SIDE || s2 == EITHER_SIDE || s1 == s2;
}

bool sameSide(Vec2f a, Vec2f b, std::vector<Vec2f>& points)
{
	Side lastSide = whichSide(a, b, points[0]);
	
	for(int i = 1; i < (int)points.size(); ++i)
	{
		Side side = whichSide(a, b, points[i]);
		
		if(!sameSide(side, lastSide))
			return false;
		
		if(side != EITHER_SIDE)
			lastSide = side;
	}
	
	return true;
}

bool convex(std::vector<Vec2f>& points)
{
	if((int)points.size() < 3)
		return false;
		
	for(int i = 0; i < (int)points.size() - 1; ++i)
		if(!sameSide(points[i], points[i + 1], points))
			return false;
	
	return sameSide(points[(int)points.size() - 1], points[0], points);
}

bool pointInPoly(Poly& poly, Vec2f p)
{
  int i, j;
  bool c = false;
  
  int nvert = poly.pointVec.size();
  
  for(i = 0, j = nvert-1; i < nvert; j = i++)
  {
  	Vec2f v0 = poly.pointVec[i];
  	Vec2f v1 = poly.pointVec[j];
  	
    if ( ((v0.y>p.y) != (v1.y>p.y)) &&
	 (p.x < (v1.x-v0.x) * (p.y-v0.y) / (v1.y-v0.y) + v0.x) )
       c = !c;
  }
  return c;
}

void addClosedPoly(std::list<Poly>& polyList, std::vector<Vec2f>& pointVec)
{
	if(polygonArea(pointVec) < 0)
		reverse(pointVec.begin(), pointVec.end());
	
    Poly poly;
    poly.pointVec = pointVec;
    polyList.push_back(poly);
}

void addWallPoly(std::list<Poly>& polyList, std::vector<Vec2f>& pointVec)
{
	if(polygonArea(pointVec) < 0)
    	reverse(pointVec.begin(), pointVec.end());

    Poly poly;
    int n = pointVec.size();
    poly.pointVec = pointVec;
	poly.pointVec.push_back(pointVec[n - 1] + (pointVec[n - 1] - pointVec[n - 2]).unitNormal() * 10);

    if(n > 2)
    	for(int i = n - 2; i > 0; i--)
            poly.pointVec.push_back(genPoint(pointVec[i - 1], pointVec[i], pointVec[i + 1], 10));

    poly.pointVec.push_back(pointVec[0] + (pointVec[1] - pointVec[0]).unitNormal() * 10);
    polyList.push_back(poly);
}

bool deleteMode()
{
	return key[KEY_LSHIFT] && key[KEY_D];
}

void drawPolyList(BITMAP* buffer, std::list<Poly>& polyList, bool snapMode)
{
	bool foundOne = false;
	
	for(std::list<Poly>::iterator i = polyList.begin(); i != polyList.end(); ++i)
	{
		if(pointInPoly(*i, Vec2f(mouse_x, mouse_y)) && deleteMode() && !foundOne)
		{ 
    		drawPoly(buffer, *i, makeacol(0, 0, 255, 100));
    		foundOne = true;
    	}
    	
    	else
			drawPoly(buffer, *i, makeacol(0, 0, 128, 100));
    }
    
    if(snapMode)
    	for(std::list<Poly>::iterator i = polyList.begin(); i != polyList.end(); ++i)
    	{
    		Poly& poly = *i;
    		
    		for(int j = 0; j < poly.pointVec.size(); ++j)
    			DrawVec2f::circle(buffer, poly.pointVec[j], SNAP_RAD, makecol(255, 255, 255));
    	}
}

bool snap(Vec2f& p, std::vector<Vec2f>& points, float rad)
{
	for(int j = 0; j < points.size(); ++j)
    	if((points[j] - p).length() <= rad)
    	{
    		p = points[j];
    		return true;
    	}
    
    return false;
}

bool snap(Vec2f& p, std::list<Poly>& polyList, float rad)
{
	for(std::list<Poly>::iterator i = polyList.begin(); i != polyList.end(); ++i)
    	if(snap(p, (*i).pointVec, rad))
    		return true;
    
    return false;
}

void drawEditedPoly(BITMAP* buffer, std::vector<Vec2f>& pointVec, Vec2f curr, bool snapped, bool valid)
{
	int n = pointVec.size();
	
	for(int i = 0; i < int(n) - 1; i++)
		DrawVec2f::line(buffer, pointVec[i], pointVec[i + 1], 0);
		
	for(int i = 0; i < n; i++)
    	DrawVec2f::circlefill(buffer, pointVec[i], 2, 0);
    
    if(n > 0)
    {
    	int col = valid ? makecol(100, 100, 100) : makecol(100, 0, 0);
    	
    	DrawVec2f::line(buffer, pointVec[n - 1], curr, col);
    	DrawVec2f::line(buffer, pointVec[0], curr, col);
    }
    
    if(snapped)
   		DrawVec2f::circle(buffer, curr, SNAP_RAD, makecol(255, 255, 255));
}

std::list<Poly>::iterator findPoly(std::list<Poly>& polyList, Vec2f p)
{
	for(auto iter = polyList.begin(); iter != polyList.end(); ++iter)
		if(pointInPoly(*iter, p))
			return iter;
	
	return polyList.end();
}

Json::Value vec2fToJson(Vec2f v)
{
	Json::Value out;
	
	out["x"] = v.x;
	out["y"] = v.y;
	
	return out;
}

Json::Value polyToJson(Poly& poly)
{
	Json::Value out;
	
	for(int i = 0; i < poly.pointVec.size(); ++i)
		out[i] = vec2fToJson(poly.pointVec[i]);
		
	return out;
}

Json::Value polyListToJson(std::list<Poly>& polyList)
{
	Json::Value out;
	int i = 0;
	
	for(Poly& poly : polyList)
	{
		out[i] = polyToJson(poly);
		++i;
	}
	
	return out;
}

Json::Value read(char const* fn)
{
	Json::Value jval;
	Json::Reader reader;
	
	std::ifstream in;
	in.open("colliders.json");
	
	if(in.good())
		reader.parse(in, jval, false);
		
	in.close();
		
	return jval;
}

void write(char const* fn, Json::Value jval)
{
	Json::FastWriter writer;	
	
    std::ofstream out; 
    out.open(fn);
    out << writer.write(jval);
    
    out.close();
    
    //myfile.open("colliders.json");
    //myfile << writer.write(root);
   /// myfile.close();


	//printf("%s", polyListToJson(polyList).c_str());
}

BITMAP* myLoadBmp(char const* path)
{
   BITMAP* bmp = load_bitmap(path, NULL);
   
   if(bmp != nullptr)
   for(int j = 0; j < bmp->h; ++j)
      for(int i = 0; i < bmp->w; ++i)
      {
         if(geta(PIXEL(bmp, i, j)) < 100)
           PIXEL(bmp, i, j) = makecol(255, 0, 255);
      }
      
    return bmp;
}

void translatePolyList(std::list<Poly>& polyList, float tx, float ty)
{
	for(Poly& poly : polyList)
		for(Vec2f& vtx : poly.pointVec)
		{
			vtx.x += tx;
			vtx.y += ty;
		}
}

int main(int argc, char *argv[])
{
    bool exit = false;

    init();
    Keyboard keyboard;

    int lastMb = 0;

    std::vector<Vec2f> pointVec;
    std::list<Poly> polyList;
    	
    BITMAP* bmp = nullptr;
    
    float offsX = 0;
	float offsY = 0;
	
	if(argc > 1)
    {
    	bmp = myLoadBmp(argv[1]);
    
		if(bmp == nullptr)
		{
			printf("Couldn't load %s", argv[1]);
			return 0;
		}
		
	  offsX = (buffer->w - bmp->w) / 2;
	  offsY = (buffer->h - bmp->h) / 2;
	}
	
    while(!exit)
    {
        if(key[KEY_ESC]) exit = true;

        int n = pointVec.size();
        
        keyboard.update();

		Vec2f nextPoint = Vec2f(mouse_x, mouse_y);
		bool canClose = snap(nextPoint, pointVec, SNAP_RAD);
		bool snapped = canClose || snap(nextPoint, polyList, SNAP_RAD);
		std::vector<Vec2f> temp = pointVec;
		temp.push_back(nextPoint);
		bool valid = convex(temp);
		
        if(mouse_b == 1 && !lastMb)
        {
        	if(deleteMode())
        	{
        		auto iter = findPoly(polyList, Vec2f(mouse_x, mouse_y));
        		if(iter != polyList.end())
					polyList.erase(iter);
        	}
        	
        	else if(valid || pointVec.size() < 3)
        	{
        		if(canClose)
        		{
        			addClosedPoly(polyList, pointVec);
        			printf("%d\n", pointVec.size());
            		pointVec.clear();
        		}
        	
        		else
            		pointVec.push_back(nextPoint);
            }
        }

        lastMb = mouse_b;

        clear_to_color(buffer, makecol(128, 128, 128));
		
		if(bmp != nullptr)
		{
			alphaOn();
			draw_sprite(buffer, bmp, offsX, offsY);
			alphaOff();
		}

		drawPolyList(buffer, polyList, key[KEY_SPACE]);
		drawEditedPoly(buffer, pointVec, nextPoint, snapped, valid);
		
        draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    if(argc > 1 && !polyList.empty())
    {
    	translatePolyList(polyList, -offsX, -offsY);
    	
    	Json::Value root = read("colliders.json");
		Json::Value collider;
	
		collider["img"] = argv[1];
    	collider["polys"] = polyListToJson(polyList);
    	root[root.size()] = collider;
    
    	write("colliders.json", root);
    }

	destroy_bitmap(bmp);
    deinit();
    return 0;
}
END_OF_MAIN()
