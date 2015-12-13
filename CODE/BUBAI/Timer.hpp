/* 
 * File:   Timer.hpp
 * Author: Stas
 *
 * Created on October 1, 2013, 11:32 AM
 */

#ifndef TIMER_HPP
#define	TIMER_HPP

#include <chrono>

class Timer {
    typedef std::chrono::high_resolution_clock high_resolution_clock;
    typedef std::chrono::milliseconds msecs;
    
    high_resolution_clock::time_point mStart;
    
    static Timer* mInstance;
    
public:
    Timer(bool run = false)
    {
        if(run)
            reset();
    }
    
    static Timer& getInstance()
    {
        if(!mInstance)
            mInstance = new Timer(true);
        return *mInstance;
    }

    void reset()
    {
        mStart = high_resolution_clock::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<msecs>(high_resolution_clock::now() - mStart).count() / 1000.0;
    }
};

Timer* Timer::mInstance = NULL;

#endif	/* TIMER_HPP */

