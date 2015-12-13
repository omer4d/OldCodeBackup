/* 
 * File:   EventDispatcher.hpp
 * Author: Stas
 *
 * Created on February 17, 2013, 11:25 PM
 */

#ifndef EVENTDISPATCHER_HPP
#define	EVENTDISPATCHER_HPP

#include "SafeEventListener.hpp"

template <class T>
class EventDispatcher {
    typedef std::list<Proxy<EventListener<T> > > ListenerList;
    typedef typename ListenerList::iterator ListenerIter;
    
    ListenerList listenerList;

public:
    void addListener(EventListener<T>& listener)
    {
        listenerList.push_back(listener.getProxy());
    }
    
    void removeListener(EventListener<T>& listener)
    {
        listenerList.remove(listener.getProxy());
    }
    
    void dispatchEvent(T const& e)
    {
        for(auto i = listenerList.begin(); i != listenerList.end();)
        {
            EventListener<T>* listener = i->getObject();
            
            if(listener)
            {
                listener->onEvent(e);
                ++i;
            }
            
            else
                i = listenerList.erase(i);
        }
    }
};

#endif	/* EVENTDISPATCHER_HPP */

