/* 
 * File:   Lexcast.hpp
 * Author: Stas
 *
 * Created on October 6, 2013, 12:08 AM
 */

#ifndef LEXCAST_HPP
#define	LEXCAST_HPP

#include <stdexcept>
#include <string>
#include <sstream>

namespace Util {
    class LexcastError: public std::runtime_error {
    public:
        LexcastError(): std::runtime_error("Bad lexical cast.")
        {
            
        }
    };
    
    template <class T>
    bool lexcast(T& out, std::string const& in)
    {
        std::stringstream converter;
        
        converter << in;
        converter >> out;
        
        return !converter.fail();
    }

    template <typename T>
    T lexcast(std::string const& in, T const& def)
    {
        T val;
        bool success = Util::lexcast<T>(val, in);

        return success ? val : def;
    }
    
    template <class T>
    T lexcast(std::string const& in)
    {
        T out;

        std::stringstream converter;
        converter << in;
        converter >> out;
        
        if(converter.fail())
            throw LexcastError();

        return out;
    }

    template <class T>
    std::string lexcast(T const& in)
    {
        std::string out;

        std::stringstream converter;
        converter << in;
        converter >> out;
        
        if(converter.fail())
            throw LexcastError();

        return out;
    }
}

#endif	/* LEXCAST_HPP */

