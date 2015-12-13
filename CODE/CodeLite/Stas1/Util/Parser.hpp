/* 
 * File:   Parser.hpp
 * Author: Stas
 *
 * Created on June 17, 2012, 4:05 AM
 */

#ifndef PARSER_HPP
#define	PARSER_HPP

#include <cstring>
#include <vector>
#include <string>
#include <map>

class ParserCommand {
public:
    
    virtual ~ParserCommand()
    {
        
    }
    virtual bool exec(std::vector<std::string> const& params, std::string& error) = 0;
};

class Parser {
    typedef std::map<std::string, ParserCommand*> Map;
    Map map;
    
    static void stringToVec(std::vector<std::string>& vec, std::string const& str)
    {
        char* cstr = new char [str.size() + 1], *p;
        
        strcpy(cstr, str.c_str());
        p = strtok(cstr, " ,");

        while(p != NULL)
        {
            vec.push_back(std::string(p));
            p = strtok(NULL, " ,");
        }

        delete[] cstr;
    }
    
public:
    ~Parser()
    {
        for(Map::iterator i = map.begin(); i != map.end(); ++i)
            delete i->second;
    }
    
    void regCommand(std::string const& cmdName, ParserCommand* cmd)
    {
        map[cmdName] = cmd;
    }
    
    void unregCommand(std::string const& cmdName)
    {
        map.erase(cmdName);
    }
    
    bool parse(std::string const& str, std::string& error)
    {
        std::vector<std::string> vec;
        stringToVec(vec, str);
        
        if(vec.size() < 1)
            return true;
        
        Map::iterator i = map.find(vec[0]);
        
        if(i == map.end())
        {
            error = "Command not found!";
            return false;
        }
        
        else
            return i->second->exec(vec, error);
    }
};

#endif	/* PARSER_HPP */

