/* 
 * File:   Entity.hpp
 * Author: Stas
 *
 * Created on January 26, 2012, 12:57 AM
 */

#ifndef ENTITY_HPP
#define	ENTITY_HPP

#include <cstring>
#include <cctype>

#include <list>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <Proxy.hpp>

#include <stdio.h>

#include "Component.hpp"

#include <luabind/luabind.hpp>

#define GAME_UNITS_PER_METER 1.0

using namespace luabind;

class Entity {
    friend class Container;
    
public:
    typedef std::list<Component*> CompList;
    
    enum Group {
        NONE, PLAYER, FRIENDLY, ENEMY, ITEM, OBJECT
    };

private:

    static float timeDelta;

    boost::shared_ptr<bool> alive;

    Entity* parent;
    bool visible, active;
    Vec3f pos;
    float rad;

    Group group;
    Component* compArr[COMPONENT_TYPE_NUM];

    Entity(const Entity&);
    Entity& operator=(const Entity&);
    
    void setParent(Entity* parent)
    {
        this->parent = parent;
    }

protected:
    virtual ~Entity()
    {
        *alive = false;

        for(int i = 0; i < COMPONENT_TYPE_NUM; i++)
            delete compArr[i];
    }

    boost::shared_ptr<bool> getAlive()
    {
        return alive;
    }

    void setGroup(Group group)
    {
        this->group = group;
    }

public:

    Entity()
    {
        this->alive.reset(new bool(true));
        this->visible = true;
        this->active = true;
        this->parent = NULL;

        this->group = NONE;
        this->rad = 0.0f;

        for(int i = 0; i < COMPONENT_TYPE_NUM; i++)
            compArr[i] = NULL;
    }

    static float getTimeDelta()
    {
        return Entity::timeDelta;
    }

    static void setTimeDelta(float dt)
    {
        Entity::timeDelta = dt;
    }

    // *******************
    // * Getters\Setters *
    // *******************

    Proxy<Entity> getProxy()
    {
        return Proxy<Entity > (this, alive);
    }

    void setPos(Vec3f pos)
    {
        this->pos = pos;
    }

    Vec3f getPos() const
    {
        return pos;
    }

    Vec3f getAbsPos() const
    {
        if(parent) return parent->getAbsPos() + pos;
        else return pos;
    }
    
    void setRad(float rad)
    {
        this->rad = rad;
    }
    
    float getRad() const
    {
        return rad;
    }

    void setActive(bool active)
    {
        this->active = active;
    }

    bool isActive() const
    {
        return active;
    }

    void setVisible(bool visible)
    {
        this->visible = visible;
    }

    bool isVisible() const
    {
        return visible;
    }

    Entity* getParent() const
    {
        return parent;
    }

    Group getGroup() const
    {
        return group;
    }

    template <class T> T* getComponent() const
    {
        return (T*)compArr[T::getClassId()];
    }

    // ***********
    // * Methods *
    // ***********

    void addComponent(Component* comp)
    {
        delete compArr[comp->getId()];
        comp->setOwner(this);
        compArr[comp->getId()] = comp;
    }

    virtual void logic()
    {
        for(int i = 0; i < COMPONENT_TYPE_NUM; i++)
            if(compArr[i])
                compArr[i]->logic();
    }
    
    virtual void render(BITMAP* buffer, float tx, float ty, float scale) const
    {
        if(!visible)
            return;
        
        Vec3f pos3 = getAbsPos();
        circlefill(buffer, (int) (tx + pos3.x * scale), (int) (ty + pos3.z * scale),
                   (int) (0.5 * 0.5 * scale), 0);
        
        for(int i = 0; i < COMPONENT_TYPE_NUM; i++)
            if(compArr[i])
                compArr[i]->render(buffer, tx, ty, scale);
    }
    
    // ***************
    // * LUA binding *
    // ***************
    
    static Proxy<Entity> createLuaProxy()
    {
        Entity* ent = new Entity();
        return ent->getProxy();
    }

    static void luaBind(lua_State* luaState)
    {
        luabind::module(luaState)
                [
                class_<Entity > ("Entity")
                .def_readwrite("visible", &Entity::visible)
                .def_readwrite("active", &Entity::active)
                .def_readwrite("pos", &Entity::pos)
                .def_readwrite("rad", &Entity::rad)

                .def("addComponent", &Entity::addComponent)
                .def("getAbsPos", &Entity::getAbsPos)
                .scope
                [
                        def("createProxy", &createLuaProxy)
                ],

                class_<Proxy<Entity> > ("EntityProxy")
                .def("expired", &Proxy<Entity>::expired)
                .def("getObject", &Proxy<Entity>::getObject)
                ];
    }
};

float Entity::timeDelta = 0.0f;

#endif	/* ENTITY_HPP */

