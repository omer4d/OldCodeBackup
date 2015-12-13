/* 
 * File:   Proxiable.hpp
 * Author: Stas
 *
 * Created on February 18, 2013, 12:33 AM
 */

#ifndef PROXIABLE_HPP
#define	PROXIABLE_HPP

#include "Proxy.hpp"

class Proxiable {
protected:
    boost::shared_ptr<bool> alive;
    
    boost::shared_ptr<bool> getAlive()
    {
        return alive;
    }
    
    Proxiable(): alive(new bool(true))
    {
        
    }
    
    ~Proxiable()
    {
        *alive = false;
    }
};

#endif	/* PROXIABLE_HPP */

