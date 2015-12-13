/* 
 * File:   TrackFollower.hpp
 * Author: Stas
 *
 * Created on January 27, 2012, 1:25 AM
 */

#ifndef TRACKFOLLOWER_HPP
#define	TRACKFOLLOWER_HPP

#include <allegro.h>
#include <list>

#include "Component.hpp"
#include "ComponentId.hpp"

#include "Entity.hpp"
#include "Vec2f.hpp"
#include "BezierPath.hpp"

#include "Event.hpp"

class TrackFollowerEvent {
public:
    enum Type {
        TARGET_SET, TARGET_REACHED
    };
    
private:
    Type type;
    
public:
    TrackFollowerEvent(Type type)
    {
        this->type = type;
    }
    
    Type getType() const
    {
        return type;
    }
};

class TrackFollower : public Component {
    float dist, speed, dir, rotSpeed;
    Vec2f desiredViewVec;
    BezierPath* bezPath;
    bool targetReached, loop;
    EventDispatcher<TrackFollowerEvent> dispatcher;
    
public:
    TrackFollower()
    {
        this->dir = 0.0;
        this->desiredViewVec = Vec2f(cos(this->dir), sin(this->dir));
        this->speed = 0.0;
        this->rotSpeed = 0.0;
        this->dist = 0.0;
        this->bezPath = NULL;
        this->targetReached = false;
        this->loop = false;
    }

    ~TrackFollower()
    {
        delete bezPath;
    }

    static ComponentId getClassId()
    {
        return TRACK_FOLLOWER_COMP;
    }

    ComponentId getId()
    {
        return TrackFollower::getClassId();
    }
    
    // *******************
    // * Getters\Setters *
    // *******************
    
    void setLoop(bool loop)
    {
        this->loop = loop;
    }
    
    EventDispatcher<TrackFollowerEvent>& events()
    {
        return dispatcher;
    }
    
    void setSpeed(float speed)
    {
        this->speed = speed;
    }

    float getSpeed() const
    {
        return speed;
    }

    void setDist(float dist)
    {
        this->dist = dist;
    }

    float getDist() const
    {
        return dist;
    }

    float getDir() const
    {
        return dir;
    }

    void setRotSpeed(float rotSpeed)
    {
        this->rotSpeed = rotSpeed;
    }

    float getRotSpeed() const
    {
        return rotSpeed;
    }

    void onTargetReached()
    {
        TrackFollowerEvent e(TrackFollowerEvent::TARGET_REACHED);
        dispatcher.dispatch(e);
    }
    
    void onTargetSet()
    {
        TrackFollowerEvent e(TrackFollowerEvent::TARGET_SET);
        dispatcher.dispatch(e);
    }
    
    void setTrack(std::list<Vec2f> const& path, float curviness)
    {
        Entity* owner = getOwner();
        
        delete bezPath;
        bezPath = new BezierPath(path, curviness);
        dist = 0.0;
        desiredViewVec = bezPath->tangent(0.0).unit();
        
        Vec2f p = bezPath->point(dist);
        owner->setPos(Vec3f(p.x, owner->getPos().y, p.y));
        
        targetReached = false;
        onTargetSet();
    }

    virtual void render(BITMAP* buffer, float tx, float ty, float scale) const
    {
        Entity* owner = getOwner();
        Vec3f pos3 = owner->getAbsPos();
        Vec2f t = Vec2f(pos3.x, pos3.z) + Vec2f(1.0 * cos(dir), 1.0 * sin(dir));
        
        if(bezPath)
            bezPath->render(buffer, tx, ty, scale, makecol(0, 0, 255));
        
        line(buffer, (int) (tx + pos3.x * scale), (int) (ty + pos3.z * scale),
                (int) (tx + t.x * scale), (int) (ty + t.y * scale), 0);
    }

    virtual void logic()
    {
        Entity* owner = getOwner();
        
        if(!targetReached)
        {
            float timeDelta = Entity::getTimeDelta();
            float dirDelta = Vec2f(cos(this->dir), sin(this->dir)).normal().dot(desiredViewVec);

            if(fabs(dirDelta) > 0.1)
                dir += dirDelta * rotSpeed * timeDelta;

            else if(bezPath)
            {
                Vec2f p = bezPath->point(dist);
                desiredViewVec = bezPath->tangent(dist).unit();

                dir = Vec2f::ang(Vec2f(1.0, 0.0), desiredViewVec, 1.0, 1.0);
                owner->setPos(Vec3f(p.x, owner->getPos().y, p.y));
                dist += speed * timeDelta;
                
                if(dist  >= bezPath->len())
                {
                    targetReached = true;
                    onTargetReached();
                    
                    if(loop)
                    {
                        targetReached = false;
                        dist = 0.0;
                    }
                }
            }
        }
    }
};

#endif	/* TRACKFOLLOWER_HPP */

