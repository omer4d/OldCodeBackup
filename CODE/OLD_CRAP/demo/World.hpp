/* 
 * File:   World.hpp
 * Author: Stas
 *
 * Created on January 27, 2012, 10:36 PM
 */

#ifndef WORLD_HPP
#define	WORLD_HPP

#include <allegro.h>
#include <list>

#include "AABB.hpp"

#include "Entity.hpp"
#include "TileMap.hpp"
#include "Container.hpp"

namespace Audio {
    class EngineManager;
}

class World: public Entity {
    typedef std::list<Vec2f> Path;
    
    TileMap tileMap;
    float tileMapScale;
    Container* container;
    
public:

    World(char const* path, float tileMapScale) : Entity()
    {
        this->container = new Container();
        
        container->setSearchEnabled(true);
        
        this->addComponent(container);
        this->tileMapScale = tileMapScale;
        this->tileMap.read(path);
    }
    
    Container* getContainer() const
    {
        return container;
    }
    
    float t2wx(float tx) const { return (tx + 0.5) * tileMapScale; }
    float t2wy(float ty) const { return (ty + 0.5) * tileMapScale; }
    int w2tx(float wx) const { return (int)(wx / tileMapScale - 0.5); }
    int w2ty(float wy) const { return (int)(wy / tileMapScale - 0.5); }
    
    void addChild(Entity* ent)
    {
        container->addChild(ent);
    }
    
    void addChild(Entity* ent, Vec3f const& pos)
    {
        container->addChild(ent, pos);
    }
    
    void killChild(Entity* ent)
    {
        container->killChild(ent);
    }
    
    AABB getBounds()
    {
        return AABB(Vec2f(t2wx(-0.5), t2wy(-0.5)),
                    Vec2f(t2wx(tileMap.w - 0.5), t2wy(tileMap.h - 0.5)));
    }
    
    
    Vec2f findMark(std::string const& name) const
    {
        int x, y;
        tileMap.find(name, x, y);
        return Vec2f(t2wx(x), t2wy(y));
    }
    
    bool findPath(Vec2f const& orig, Vec2f const& dest, Path& path) const
    {
        tileMap.findPath(w2tx(orig.x), w2ty(orig.y), w2tx(dest.x), w2ty(dest.y), path);
        
        for(Path::iterator i = path.begin(); i != path.end(); ++i)
        {
            i->x = t2wx(i->x);
            i->y = t2wy(i->y);
        }
            
        *(--path.end()) = orig;
        *path.begin() = dest;
    }
    
    void logic()
    {
        container->logic();
    }
    
    void render(BITMAP* buffer, float tx, float ty, float scale) const
    {
        tileMap.render(buffer, tx, ty, scale * tileMapScale);
        container->render(buffer, tx, ty, scale);
    }
};



#endif	/* WORLD_HPP */

