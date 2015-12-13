/* 
 * File:   Event.hpp
 * Author: Stas
 *
 * Created on February 12, 2012, 6:09 PM
 */

#ifndef EVENT_HPP
#define	EVENT_HPP

#include <list>
#include "Proxy.hpp"

template <class Event>
class EventListener {
public:
    virtual bool onEvent(Event& event) = 0;
};

template <class Event>
class EventDispatcher {
    typedef Proxy< EventListener<Event > > ListenerProxy;
    typedef std::list<ListenerProxy > ListenerList;
    ListenerList listenerList;
    
public:

    void addListener(ListenerProxy p)
    {
        listenerList.push_back(p);
    }
    
    void removeListener(ListenerProxy p)
    {
        listenerList.remove(p);
    }
    
    void dispatch(Event& event)
    {
        for(typename ListenerList::iterator i = listenerList.begin(); i != listenerList.end();)
            if(!i->expired())
            {
                i->getObject()->onEvent(event);
                ++i;
            }
        
            else
                i = listenerList.erase(i);
    }
};

#endif	/* EVENT_HPP */

