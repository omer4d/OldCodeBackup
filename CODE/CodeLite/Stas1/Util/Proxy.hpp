/* 
 * File:   Proxy.hpp
 * Author: Stas
 *
 * Created on January 31, 2012, 1:31 AM
 */

#ifndef PROXY_HPP
#define	PROXY_HPP

#include <boost/shared_ptr.hpp>

template <class T>
class Proxy {
protected:
    boost::shared_ptr<bool> alive;
    T* object;
    
public:
    Proxy(): alive(new bool(false))
    {
        object = NULL;
    }
    
    Proxy(T* object, boost::shared_ptr<bool> alive)
    {
        this->alive = alive;
        this->object = object;
    }
    
    template <class U> Proxy(Proxy<U> other)
    {
        this->alive = other._getAlive();
        this->object = other.getObject();
    }
    
    bool operator==(Proxy other)
    {
        return this->object == other.object;
    }
    
    bool expired() const
    {
        return !*alive;
    }
    
    boost::shared_ptr<bool> _getAlive() const
    {
        return alive;
    }
    
    T* getObject() const
    {
        return *alive ? object : NULL;
    }
};

#endif	/* PROXY_HPP */

