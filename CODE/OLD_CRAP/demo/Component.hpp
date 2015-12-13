/* 
 * File:   Component.hpp
 * Author: Stas
 *
 * Created on February 4, 2012, 5:08 PM
 */

#ifndef COMPONENT_HPP
#define	COMPONENT_HPP

#include "SoundEngine/AudioEngine.hpp"
#include "ComponentId.hpp"
#include <luabind/luabind.hpp>

using namespace luabind;

class Entity;

class Component {
    Entity* owner;
    boost::shared_ptr<bool> alive;
    
protected:
    boost::shared_ptr<bool> getAlive()
    {
        return alive;
    }
    
public:
    Component(): alive(new bool(true))
    {
    }
    
    virtual ~Component()
    {
        *alive = false;
    }
    
    virtual ComponentId getId() = 0;
    
    void setOwner(Entity* owner)
    {
        this->owner = owner;
    }
    
    Entity* getOwner() const
    {
        return this->owner;
    }
    
    virtual void logic()
    {    
    }
    
    virtual void render(BITMAP* buffer, float tx, float ty, float scale) const
    {
    }
    
    // ***************
    // * LUA binding *
    // ***************
    
    static void luaBind(lua_State* luaState)
    {
        luabind::module(luaState)
                [
                class_<Component > ("Component")
                .def_readonly("owner", &Component::owner)
                ];
    }
};

#endif	/* COMPONENT_HPP */