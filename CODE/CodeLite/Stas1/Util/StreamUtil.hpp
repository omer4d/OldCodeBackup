/* 
 * File:   StreamUtil.hpp
 * Author: Stas
 *
 * Created on January 31, 2013, 4:47 AM
 */

#ifndef STREAMUTIL_HPP
#define	STREAMUTIL_HPP

#include <istream>
#include <sstream>
#include <string>
#include <fstream>

#include "MissingFileException.hpp"

bool readIntoString(std::string& str, std::istream& in)
{
    std::stringstream buffer;
    buffer << in.rdbuf();
    str = buffer.str();
}

std::string readTextFile(char const* path)
{
    std::ostringstream contents;
    std::ifstream file(path);
    
    if (!file.is_open())
    {
        throw MissingFileException(path);
    }
    
    contents << file.rdbuf();
    return (contents.str());
}

int parseHexColor(char const* code)
{
    std::stringstream converter;
    int col;

    if(code[0] == '#') converter << &code[1];
    else converter << code;

    converter >> std::hex >> col;
    return col;
}

#endif	/* STREAMUTIL_HPP */

