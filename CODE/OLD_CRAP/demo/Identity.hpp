/* 
 * File:   Identity.hpp
 * Author: Stas
 *
 * Created on February 4, 2012, 4:37 PM
 */

#ifndef IDENTITY_HPP
#define	IDENTITY_HPP

#include "ComponentId.hpp"
#include "Component.hpp"
#include "Parser.hpp"

#include "Shared.hpp"

typedef std::list<std::string const*> StrConstPtrList;
typedef std::list<std::string> StringList;

class Identity: public Component {
    StringVec nameVec, descVec;

public:

    static ComponentId getClassId()
    {
        return IDENTITY_COMP;
    }
    
    ComponentId getId()
    {
        return Identity::getClassId();
    }

    StringVec const& getNameVec() const
    {
        return nameVec;
    }

    StringVec const& getDescVec() const
    {
        return descVec;
    }

    void setNames(std::string const& names)
    {
        nameVec.clear();
        Parser::stringToVec(names, nameVec);
        
        if(Shared::getParser())
            for(int i = 0; i < nameVec.size(); i++)
                Shared::getParser()->pushName(nameVec[i]);
    }

    void setDescs(std::string const& descs)
    {
        descVec.clear();
        Parser::stringToVec(descs, descVec);
        
        if(Shared::getParser())
            for(int i = 0; i < descVec.size(); i++)
                Shared::getParser()->pushDesc(descVec[i]);
    }

    bool match(std::string const& name, StringList const& descList) const
    {
        if(!Parser::vecContainsString(this->nameVec, name)) return false;

        for(StringList::const_iterator i = descList.begin(); i != descList.end(); ++i)
            if(!Parser::vecContainsString(this->descVec, *i))
                return false;

        return true;
    }
    
    void render(BITMAP* buffer, float tx, float ty, float scale) const
    {
        Vec3f pos3 = getOwner()->getAbsPos();
        if(nameVec.size() > 0)
            textprintf_ex(buffer, font, (int) (tx + pos3.x * scale), (int) (ty + pos3.z * scale) - 8, 0, -1, "%s", nameVec[0].c_str());
    }
    
    // ************
    // * LUA bind *
    // ************
    
    static Identity* create(std::string const& names, std::string const& descs)
    {
        Identity* t = new Identity();
        t->setNames(names);
        t->setDescs(descs);
        return t;
    }

    static void luaBind(lua_State* luaState)
    {
        luabind::module(luaState)
                [
                class_<Identity, Component > ("Identity")

                .scope
                [
                        def("create", &create)
                ]
                ];
    }
};

#endif	/* IDENTITY_HPP */