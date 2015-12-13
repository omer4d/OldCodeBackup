/* 
 * File:   Event.hpp
 * Author: Stas
 *
 * Created on October 6, 2013, 6:15 PM
 */

#ifndef EVENT_HPP
#define	EVENT_HPP

#include <list>

template <class T>
class EventListener {
public:
    virtual void onEvent(T const&) = 0;
};


template <class T>
class EventDispatcher {
    typedef std::list<EventListener<T>* > ListenerList;
    typedef typename ListenerList::iterator ListenerIter;
    
    ListenerList mListenerList;

public:
    void addListener(EventListener<T>& listener)
    {
        mListenerList.push_back(&listener);
    }
    
    void removeListener(EventListener<T>& listener)
    {
        mListenerList.remove(&listener);
    }
    
    void dispatchEvent(T const& e)
    {
        for(auto i = mListenerList.begin(); i != mListenerList.end(); ++i)
        {
            (*i)->onEvent(e);
        }
    }
};

#endif	/* EVENT_HPP */

