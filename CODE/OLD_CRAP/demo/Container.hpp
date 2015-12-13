/* 
 * File:   Container.hpp
 * Author: Stas
 *
 * Created on February 4, 2012, 1:21 PM
 */

#ifndef CONTAINER_HPP
#define	CONTAINER_HPP

#include "ComponentId.hpp"
#include "Component.hpp"
#include "Identity.hpp"

#include "Entity.hpp"
#include "Parser.hpp"

class Container : public Component {
    typedef std::list<Entity*> EntityList;
    typedef std::list<Entity*>::iterator EntityIter;
    
    EntityList childList;
    bool searchEnabled, putEnabled;
    float maxRad;
    int itemNum, maxItems;
    
public:
    enum PutResult {
        PUT_DISABLED, PUT_SUCCESS, PUT_TOO_BIG, PUT_IS_FULL
    };
    
    Container(int maxItems=10, float maxRad=0, bool searchEnabled=false, bool putEnabled=false)
    {
        this->searchEnabled = searchEnabled;
        this->itemNum = 0;
        this->maxItems = maxItems;
        this->maxRad = maxRad;
        this->putEnabled = putEnabled;
    }
    
    static ComponentId getClassId()
    {
        return CONTAINER_COMP;
    }

    ComponentId getId()
    {
        return Container::getClassId();
    }
    
    // *******************
    // * Getters\Setters *
    // *******************

    void setMaxItems(int maxItems)
    {
        this->maxItems = maxItems;
    }

    int getMaxItems() const
    {
        return maxItems;
    }

    void setMaxRad(float maxRad)
    {
        this->maxRad = maxRad;
    }

    float getMaxRad() const
    {
        return maxRad;
    }


    void setPutEnabled(bool putEnabled)
    {
        this->putEnabled = putEnabled;
    }

    bool isPutEnabled() const
    {
        return putEnabled;
    }

    void setSearchEnabled(bool searchEnabled)
    {
        this->searchEnabled = searchEnabled;
    }

    bool isSearchEnabled() const
    {
        return searchEnabled;
    }
    
    // ***********
    // * Methods *
    // ***********

    virtual int findSingle(Entity*& res, std::string const& name, StringList const& descList) const
    {
        int found = 0;
        
        if(!searchEnabled)
            return 0;

        for(EntityList::const_iterator i = childList.begin(); i != childList.end(); ++i)
        {
            Container* container = (*i)->getComponent<Container > ();
            Identity* identity = (*i)->getComponent<Identity > ();

            if(identity && identity->match(name, descList))
            {
                res = *i;
                found++;
            }

            if(found > 1) break;

            if(container)
                found += container->findSingle(res, name, descList);
            
            if(found > 1) break;
        }

        return found;
    }

    virtual int find(std::list<Entity*>& res, std::string const& name, StringList const& descList) const
    {
        int found = 0;
        
        if(!searchEnabled)
            return 0;

        for(EntityList::const_iterator i = childList.begin(); i != childList.end(); ++i)
        {
            Container* container = (*i)->getComponent<Container > ();
            Identity* identity = (*i)->getComponent<Identity > ();
            
            if(identity && identity->match(name, descList))
            {
                res.push_back(*i);
                found++;
            }
            
            if(container)
            found += container->find(res, name, descList);
        }

        return found;
    }

    virtual ~Container()
    {
        while(!childList.empty())
        {
            delete childList.back();
            childList.pop_back();
        }
    }

    void killChild(Entity* ent)
    {
        for(EntityList::iterator i = childList.begin(); i != childList.end(); ++i)
            if(*i == ent)
            {
                delete ent;
                break;
            }
    }

    void addChild(Entity* ent)
    {
        if(ent == NULL)
            return;
        for(EntityIter i = childList.begin(); i != childList.end(); ++i)
            if(*i == ent)
                return;
        
        Entity* oldOwner = ent->getParent();
        Container* oldContainer = NULL;
        if(oldOwner)
            oldContainer = oldOwner->getComponent<Container>();
        
        if(oldContainer)
            oldContainer->childList.remove(ent);
            
        childList.push_back(ent);
        ent->setParent(getOwner());
    }
    
    void addChild(Entity* ent, Vec3f const& pos)
    {
        if(ent == NULL)
            return;
        ent->setPos(pos);
        addChild(ent);
    }
    
    PutResult put(Entity* ent)
    {
        if(!putEnabled)
            return PUT_DISABLED;
        else if(ent->getRad() > maxRad)
            return PUT_TOO_BIG;
        else if(itemNum == maxItems)
            return PUT_IS_FULL;
        else
        {
            ent->setPos(Vec3f());
            addChild(ent);
            return PUT_SUCCESS;
        }
    }
    
    void logic()
    {
        for(EntityList::iterator i = childList.begin(); i != childList.end(); ++i)
            (*i)->logic();
    }

    void render(BITMAP* buffer, float tx, float ty, float scale) const
    {
        for(EntityList::const_iterator i = childList.begin(); i != childList.end(); ++i)
            (*i)->render(buffer, tx, ty, scale);
    }
    
    // ************
    // * LUA bind *
    // ************

    static Container* create()
    {
        return new Container();
    }

    static void luaBind(lua_State* luaState)
    {
        luabind::module(luaState)
                [
                class_<Container, Component > ("Container")
                .def("addChild", (void(Container::*)(Entity*))&Container::addChild)
                .def("addChild", (void(Container::*)(Entity*, Vec3f const&))&Container::addChild)
        
                .scope
                [
                        def("create", &create)
                ]
                ];
    }
};


#endif	/* CONTAINER_HPP */

