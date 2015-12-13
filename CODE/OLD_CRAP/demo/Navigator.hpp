/* 
 * File:   Navigator.hpp
 * Author: Stas
 *
 * Created on January 27, 2012, 10:35 PM
 */

#ifndef NAVIGATOR_HPP
#define	NAVIGATOR_HPP

#include <string>
#include <list>

#include "TrackFollower.hpp"
#include "Vec2f.hpp"

class Navigator : public Component {
    World* world;
    
public:

    Navigator(World* world)
    {
        this->world = world;
    }
    
    static ComponentId getClassId()
    {
        return NAVIGATOR_COMP;
    }
    
    ComponentId getId()
    {
        return Navigator::getClassId();
    }
    
    World* getWorld() const
    {
        return world;
    }
    
    bool goTo(Vec2f const& dest, float curviness)
    {
        bool success = false;
        Entity* owner = getOwner();
        TrackFollower* trackFollower = owner->getComponent<TrackFollower>();
        
        if(world != NULL && trackFollower != NULL && owner->getParent() == world)
        {
            std::list<Vec2f> path;
            Vec3f pos = owner->getPos();

            success = world->findPath(Vec2f(pos.x, pos.z), dest, path);
            trackFollower->setTrack(path, curviness);
        }

        return success;
    }
};

#endif	/* NAVIGATOR_HPP */

