/* 
 * File:   ResourceMap.hpp
 * Author: Stas
 *
 * Created on February 8, 2012, 1:39 AM
 */

#ifndef RESOURCEMAP_HPP
#define	RESOURCEMAP_HPP

#include <string>
#include <map>
#include <fstream>
#include <exception>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "Logger.hpp"
#include "ResourceManager.hpp"

typedef std::map<std::string, std::vector<std::string> > ResourceMap;

class ResourceMapParserException: public std::exception {
    char const* whatStr;
    
    public:
        ResourceMapParserException(char const* whatStr)
        {
            this->whatStr = whatStr;
        }
        
        const char* what() const throw()
        {
            return whatStr;
        }
        
        ~ResourceMapParserException() throw()
        {
        }
};

class ResourceMapParser {
    std::ifstream in;
    char buff[1000];
    
    void fail(char const* reason)
    {
        in.close();
        throw ResourceMapParserException(reason);
    }
    
    bool validChar(char c)
    {
        return c == '"' || c == '{' || c == '}';
    }
    
    char nextChar()
    {
        char c = 0;
        
        while(!validChar(c))
        {
            if(!in.good())
                return 0;
            in.read(&c, 1);
        }
        
        return c;
    }
    
    void readString(std::string& dest)
    {
        int n;
        char c = 0;
        
        for(n = 0; in.good() && c != '"'; n++)
        {
            in.read(&c, 1);
            buff[n] = c;
        }
        
        if(c != '"')
            fail("Syntax error: expected end of string");
        dest.assign(buff, n - 1);
    }
    
    enum State {
        EXPECT_KEY, GOT_KEY, EXPECT_FIRST_VALUE, EXPECT_MORE_VALUES, SUCCESS
    };
    
public:
    ResourceMapParser(ResourceMap& resMap, char const* path): in(path, std::ios::in | std::ios::binary)
    {
        if(!in.good())
            fail("Couldn't load file");
        
        std::string key, value;
        
        State state = EXPECT_KEY;
        char curr;
        
        while(state != SUCCESS)
        {
            curr = nextChar();
            
            switch(state)
            {
                case EXPECT_KEY:
                    if(curr == 0)
                        state = SUCCESS;
                    else if(curr == '"')
                    {
                        state = GOT_KEY;
                        readString(key);
                    }
                    
                    else
                        fail("Syntax error: expected key");
                    
                    break;
                    
                case GOT_KEY:
                    if(curr == '"')
                    {
                        state = EXPECT_KEY;
                        readString(value);
                        resMap[key].push_back(value);
                    }
                    
                    else if(curr == '{')
                    {
                        state = EXPECT_FIRST_VALUE;
                    }
                        
                    else
                        fail("Syntax error: expected a value or '{'");
                    break;
                    
                case EXPECT_FIRST_VALUE:
                    if(curr == '"')
                    {
                        state = EXPECT_MORE_VALUES;
                        readString(value);
                        resMap[key].push_back(value);
                    }

                    else
                        fail("Syntax error: expected a value");
                                            
                    break;
                    
                case EXPECT_MORE_VALUES:
                    if(curr == '"')
                    {
                        state = EXPECT_MORE_VALUES;
                        readString(value);
                        resMap[key].push_back(value);
                    }
                    
                    else if(curr == '}')
                    {
                        state = EXPECT_KEY;
                    }
                    
                    else
                        fail("Syntax error: expected a value or '}'");
                    
                    break;
            }
        }
    }
};

class ResourceMapHandle {
    boost::shared_ptr<ResourceMap> sptr;
    
public:
    ResourceMapHandle(): sptr(new ResourceMap())
    {
    }
    
    ResourceMapHandle(char const* path): sptr(new ResourceMap())
    {
        ResourceMapParser parser(*sptr, path);
    }
    
    ResourceMap::iterator begin()
    {
        return sptr->begin();
    }
    
    ResourceMap::iterator end()
    {
        return sptr->end();
    }
    
    boost::shared_ptr<ResourceMap> getSptr()
    {
        return sptr;
    }
};


#endif	/* RESOURCEMAP_HPP */

