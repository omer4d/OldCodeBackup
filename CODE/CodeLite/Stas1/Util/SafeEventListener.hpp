/* 
 * File:   EventListener.hpp
 * Author: Stas
 *
 * Created on February 17, 2013, 11:22 PM
 */

#ifndef EVENTLISTENER_HPP
#define	EVENTLISTENER_HPP

#include <boost/shared_ptr.hpp>
#include "Proxy.hpp"

template <class T>
class EventListener {
    boost::shared_ptr<bool> alive;

protected:
    boost::shared_ptr<bool> getAlive()
    {
        return alive;
    }

public:
    EventListener(): alive(new bool(true))
    {
    }
    
    virtual ~EventListener()
    {
        *alive = false;
    }
    
    virtual void onEvent(T const& e) = 0;
    
    Proxy<EventListener > getProxy()
    {
        return Proxy<EventListener > (this, alive);
    }
};

#endif	/* EVENTLISTENER_HPP */

