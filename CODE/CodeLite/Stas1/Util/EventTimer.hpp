/* 
 * File:   EventTimer.hpp
 * Author: Stas
 *
 * Created on October 1, 2013, 11:32 AM
 */

#ifndef EVENTTIMER_HPP
#define	EVENTTIMER_HPP

#include <list>
#include <functional>
#include "Timer.hpp"

class EventTimer {
    struct Event {
        std::function<void()> mFunc;
        double mTime;
        
        Event(double time, std::function<void()> func): mTime(time), mFunc(func)
        {
        }
    };
    
    Timer mTimer;
    std::list<Event> mEventList;
    static EventTimer* mInstance;
    
public:
   
    EventTimer(): mTimer(true)
    {
        
    }
    
    static EventTimer& getInstance()
    {
        if(!mInstance)
            mInstance = new EventTimer();
        return *mInstance;
    }
    
    void push(double delay, std::function<void()> func)
    {
        double time = mTimer.elapsed();
        Event e(time + delay, func);
        
        for(auto i = mEventList.begin(); i != mEventList.end(); ++i)
        {
            if(i->mTime > e.mTime)
            {
                mEventList.insert(i, e);
                return;
            }
        }
        mEventList.push_back(e);
    }
    
    void tick()
    {
        double time = mTimer.elapsed();
        
        while(!mEventList.empty() && mEventList.begin()->mTime <= time)
        {
            mEventList.begin()->mFunc();
            mEventList.pop_front();
        }
    }
    
    double elapsed() const
    {
        return mTimer.elapsed();
    }
};

EventTimer* EventTimer::mInstance = NULL;

#endif	/* EVENTTIMER_HPP */

