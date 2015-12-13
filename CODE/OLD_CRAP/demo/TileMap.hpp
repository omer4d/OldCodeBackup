/* 
 * File:   TileMap.hpp
 * Author: Stas
 *
 * Created on January 24, 2012, 5:00 PM
 */

#ifndef TILEMAP_HPP
#define	TILEMAP_HPP

#include <allegro.h>

#include <string>
#include <list>
#include <fstream>

#include "InputStream.hpp"
#include "Vec2f.hpp"

struct Marker {
    std::string id, name, description;
    int color;
    
    void readAscii(InputStream& in, std::string& string)
    {
        char buff[1000];
        int size;
        in.read(&size, 4);
        in.readNaive(buff, size);
        string = buff;
    }
    
    void read(InputStream& in)
    {  
        readAscii(in, id);
        readAscii(in, name);
        readAscii(in, description);
        in.read(&color, 4);
    }
};

struct Tile {
    int occupantNum;
    int* occupantArray;
    char solid;
    
    Tile()
    {
        occupantArray = NULL;
    }
    
    ~Tile()
    {
        if(occupantArray) delete[] occupantArray;
    }
    
    bool contains(int n)
    {
        for(int i = 0; i < occupantNum; i++)
            if(occupantArray[i] == n)
                return true;
        return false;
    }

    int color(Marker markerArray[])
    {
        if(occupantArray == NULL)
            return solid ? makecol(70, 70, 70) : makecol(240, 240, 240);

        int r = 0, g = 0, b = 0, c = 0;

        for(int i = 0; i < occupantNum; i++)
        {
            int col = markerArray[occupantArray[i]].color;
            r += getr(col);
            g += getg(col);
            b += getb(col);
            c++;
        }

        r /= c;
        g /= c;
        b /= c;

        if(solid) return makecol((int) (r * 0.3), (int) (g * 0.3), (int) (b * 0.3));
        else return makecol(r, g, b);
    }
    
    void read(InputStream& in)
    {
        in.read(&occupantNum, 4);
        if(occupantNum)
        {
            occupantArray = new int[occupantNum];
            for(int i = 0; i < occupantNum; i++)
                in.read(&occupantArray[i], 4);
        }
        in.read(&solid, 1);
    }
};

struct TileMap {

    struct Node {

        enum Status {
            UNVISITED, QUEUED, CLOSED
        };

        unsigned short x, y, dist;
        Status status;
        Node* parent;

        void reset()
        {
            dist = 65535;
            status = UNVISITED;
            parent = NULL;
        }

        Node()
        {
            reset();
        }

        Node(int x, int y)
        {
            this->x = x;
            this->y = y;
            reset();
        }
    };
    
    Tile** tileMatrix;
    Node** nodeMatrix;
    Marker* markerArray;
    int markerNum, w, h;

    
    void reset(int w, int h)
    {
        
    }
    
    TileMap()
    {
        tileMatrix = NULL;
        markerArray = NULL;
        w = 0;
        h = 0;
    }
    
    ~TileMap()
    {
        delete[] markerArray;
        for(int x = 0; x < w; x++)
        {
            delete[] tileMatrix[x];
            delete[] nodeMatrix[x];
        }
        
        delete[] tileMatrix;
        delete[] nodeMatrix;
    }

    bool find(std::string name, int& rx, int& ry) const
    {
        for(int i = 0; i < markerNum; i++)
            if(markerArray[i].name == name)
            {
                for(int y = 0; y < h; y++)
                    for(int x = 0; x < w; x++)
                        if(tileMatrix[x][y].contains(i))
                        {
                            rx = x;
                            ry = y;
                            return true;
                        }
                
                return false;
            }

        return false;
    }
    
    bool read(char const* path)
    {
        std::ifstream file("maps/test.map", std::ios::in | std::ios::binary);
        InputStream in(file, true);
        
        if(!file.good())
            return false;
        
        in.read(&markerNum, 4);
        markerArray = new Marker[markerNum];
        
        for(int i = 0; i < markerNum; i++)
            markerArray[i].read(in);
        
        in.read(&w, 4);
        in.read(&h, 4);
        
        tileMatrix = new Tile*[w];
        nodeMatrix = new Node*[w];
        
        for(int x = 0; x < w; x++)
        {
            tileMatrix[x] = new Tile[h];
            nodeMatrix[x] = new Node[h];
        }

        for(int y = 0; y < h; y++)
            for(int x = 0; x < w; x++)
            {
                tileMatrix[x][y].read(in);
                nodeMatrix[x][y].reset();
                nodeMatrix[x][y].x = x;
                nodeMatrix[x][y].y = y;
            }
        
        file.close();
        return true;
    }
    
    void render(BITMAP* buffer, float tx, float ty, float scale) const
    {
        for(int y = 0; y < h; y++)
            for(int x = 0; x < w; x++)
                rectfill(buffer, (int)(tx + x * scale), (int)(ty + y * scale),
                        (int)(tx + (x + 1) * scale), (int)(ty + (y + 1) * scale),
                    tileMatrix[x][y].color(markerArray));
    }
    

    
    bool walkable(int x, int y) const
    {
        return x >= 0 && x < w && y >= 0 && y < h && !tileMatrix[x][y].solid;
    }
    
    bool walkableDiag(int x0, int y0, int dx, int dy) const
    {
        return walkable(x0 + dx, y0 + dy) && walkable(x0 + dx, y0) && walkable(x0, y0 + dy);
    }

    bool lineOfSight(int x0, int y0, int x1, int y1) const
    {
        int dx = abs(x1 - x0), dy = abs(y1 - y0);
        int sx, sy;

        if(x0 < x1) sx = 1;
        else sx = -1;

        if(y0 < y1) sy = 1;
        else sy = -1;

        int err = dx - dy;

        while(x0 != x1 || y0 != y1)
        {
            if(tileMatrix[x0][y0].solid)
                return false;

            int e2 = 2 * err;

            if(e2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }
            if(e2 < dx)
            {
                err += dx;
                y0 += sy;
            }
        }

        return true;
    }
    
    bool findPath(int x0, int y0, int x1, int y1, std::list<Vec2f>& pointList/*, BITMAP* buffer, int size*/) const
    {
        std::list<Node*> queue;
        Node* current = NULL;
        
        int dx[8] = {0, 0, 1, -1, 1, -1, 1, -1},
            dy[8] = {-1, 1, 0, 0, 1, -1, -1, 1},
            weight[8] = {10, 10, 10, 10, 14, 14, 14, 14};
        
        if(x0 == x1 && y0 == y1)
        {
            pointList.push_back(Vec2f(x1, y1));
            pointList.push_back(Vec2f(x0, y0));
            return true;
        }
        
        for(int y = 0; y < h; y++)
            for(int x = 0; x < w; x++)
                nodeMatrix[x][y].reset();

        queue.push_back(&nodeMatrix[x0][y0]);
        
        while(!queue.empty())
        {
            for(std::list<Node*>::iterator i = queue.begin(); i != queue.end(); ++i)
                
            
            current = queue.front();
            queue.pop_front();
            current->status = Node::CLOSED;
            unsigned short int &x = current->x, &y = current->y;
            
            if(x == x1 && y == y1)
                break;

            for(int i = 0; i < 8; i++)
            {
                if(walkable(x + dx[i], y + dy[i]) && (i < 4 || walkableDiag(x, y, dx[i], dy[i])))
                {
                    Node* dest = &nodeMatrix[x + dx[i]][y + dy[i]];
                    
                    switch(dest->status)
                    {
                        case Node::UNVISITED:
                            dest->dist = current->dist + weight[i];
                            dest->parent = current;
                            dest->status = Node::QUEUED;
                            queue.push_back(dest);
                            break;
                            
                        case Node::QUEUED:
                            if(dest->dist > current->dist + weight[i])
                            {
                                dest->dist = current->dist + weight[i];
                                dest->parent = current;
                            }
                    }
                }
            }
            

        }
        
        bool success = current->x == x1 && current->y == y1;
        
        Node* last = current, *n;
        pointList.push_back(Vec2f(last->x, last->y));
        //circlefill(buffer, last->x * size + size / 2, last->y * size + size / 2, 2, makecol(0, 0, 255));

        for(n = last->parent; n->parent != NULL; n = n->parent)
        {
            //circlefill(buffer, n->x * size + size / 2, n->y * size + size / 2, 2, makecol(255, 0, 0));
            
            if(!lineOfSight(last->x, last->y, n->parent->x, n->parent->y))
            {
                pointList.push_back(Vec2f(n->x, n->y));
                //circlefill(buffer, n->x * size + size / 2, n->y * size + size / 2, 2, makecol(0, 0, 255));
                last = n;
            }
        }
        
        //circlefill(buffer, n->x * size + size / 2, n->y * size + size / 2, 2, makecol(0, 0, 255));
        pointList.push_back(Vec2f(n->x, n->y));
        
        return success;
    }
};

#endif	/* TILEMAP_HPP */

