/* 
 * File:   MissingFileException.hpp
 * Author: Stas
 *
 * Created on February 6, 2012, 8:20 PM
 */

#ifndef MISSINGFILEEXCEPTION_HPP
#define	MISSINGFILEEXCEPTION_HPP

#include <string>
#include <exception>

class MissingFileException: public std::exception {
    std::string path;
    
    public:
        MissingFileException(std::string const& path)
        {
            this->path = path;
        }
        
        const char* what() const throw()
        {
            return (std::string("File not found: ") + path).c_str();
        }
        
        ~MissingFileException() throw()
        {
        }
};

#endif	/* MISSINGFILEEXCEPTION_HPP */

