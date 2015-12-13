/* 
 * File:   Takable.hpp
 * Author: Stas
 *
 * Created on February 5, 2012, 6:57 PM
 */

#ifndef TAKABLE_HPP
#define	TAKABLE_HPP

#include "Shared.hpp"
#include "ComponentId.hpp"
#include "Component.hpp"
#include "SoundComponent.hpp"

//#include <luabind/luabind.hpp>

using namespace luabind;

class Takable: public Component {
public:
    static ComponentId getClassId()
    {
        return TAKABLE_COMP;
    }
    
    ComponentId getId()
    {
        return Takable::getClassId();
    }
    
    void onTake()
    {
        Entity* owner = this->getOwner();
        SoundComponent* soundComponent = owner->getComponent<SoundComponent>();
        
        owner->setActive(false);
        owner->setVisible(false);
        if(soundComponent)
            soundComponent->playRandom("take", Audio::AMBIENT_QUALITY);
    }
    
    void onDrop()
    {
        Entity* owner = this->getOwner();
        SoundComponent* soundComponent = owner->getComponent<SoundComponent>();
        
        owner->setActive(true);
        owner->setVisible(true);
        if(soundComponent)
            soundComponent->playRandom("drop", Audio::AMBIENT_QUALITY);
    }
    
    // ****************
    // * LUA bindings *
    // ****************

    static Takable* create()
    {
        return new Takable();
    }

    static void luaBind(lua_State* luaState)
    {
        luabind::module(luaState)
                [
                class_<Takable, Component > ("Takable")

                .scope
                [
                        def("create", &create)
                ]
                ];
    }
};

#endif	/* TAKABLE_HPP */