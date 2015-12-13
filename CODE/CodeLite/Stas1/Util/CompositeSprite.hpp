/* 
 * File:   CompositeSprite.hpp
 * Author: Stas
 *
 * Created on July 2, 2012, 2:21 AM
 */

#ifndef COMPOSITESPRITE_HPP
#define	COMPOSITESPRITE_HPP

#include <json/json.h>
#include <map>
#include <stdexcept>

#include "SpriteBase.hpp"
#include "Mat9f.hpp"
#include "CompositeSpriteElement.hpp"

class CompositeSprite: public SpriteBase {
    typedef std::map<std::string, CompositeSpriteElement*> ElemMap;
    
    CompositeSpriteElement* root;
    ElemMap elemMap;
    
    void addToMap(CompositeSpriteElement* elem)
    {
        elemMap[elem->name] = elem;
        
        for(CompositeSpriteElement::ChildIter i = elem->childList.begin(); i != elem->childList.end(); ++i)
            addToMap(*i);
    }
    
    void throwParseException(char const* path, char const* reason)
    {
        throw std::runtime_error(std::string("Could not parse model file: '") +
                                 path + "(" + reason + ")");
    }
    
public:
    CompositeSprite()
    {
        throw std::runtime_error("Empy model constructor called!");
    }
    
    CompositeSprite(CompositeSpriteElement* root)
    {
        this->root = root;
        addToMap(root);
    }
    
    CompositeSprite(char const* path)
    {
        std::ifstream file;
        file.open(path);

        if(!file.is_open())
        {
            throw std::runtime_error(std::string("Model not found: '") + path + "'");
        }

        Json::Value root;
        Json::Reader reader;

        if(!reader.parse(file, root))
        {
            throwParseException(path, reader.getFormattedErrorMessages().c_str());
        }

        file.close();
        
        CompositeSpriteElement* elem = CompositeSpriteElement::create(path, root);
        
        if(!elem)
            throwParseException(path, "No valid bones!");
        
        this->root = elem;
        addToMap(elem);
    }
    
    ~CompositeSprite()
    {
        delete root;
    }
    
    CompositeSprite* deepCopy() const
    {
        return new CompositeSprite(root->deepCopy());
    }
    
    CompositeSpriteElement* getElement(char const* name)
    {
        ElemMap::iterator i = elemMap.find(name);
        
        if(i != elemMap.end())
            return i->second;
        else
            return NULL;
    }
    
    void tick(float dt)
    {
        root->tick(dt);
    }
    
    void render(Mat9f const& mat) const
    {
        root->render(mat);
    }
    
    void debug() const
    {
        root->debug();
    }
};

#endif	/* COMPOSITESPRITE_HPP */

