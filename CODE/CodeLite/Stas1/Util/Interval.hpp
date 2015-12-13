/* 
 * File:   Interval.hpp
 * Author: Stas
 *
 * Created on June 27, 2012, 8:57 AM
 */

#ifndef INTERVAL_HPP
#define	INTERVAL_HPP

struct Interval {
    float min, max;

    Interval()
    {
        this->min = 0;
        this->max = 0;
    }

    Interval(float min, float max)
    {
        this->min = min;
        this->max = max;
    }

    static bool intersect(Interval const& i0, Interval const& i1)
    {
        return !(i0.min > i1.max || i1.min > i0.max);
    }

    static bool intersect(Interval const& i0, Interval const& i1, float& depth)
    {
        float d0 = i0.max - i1.min;
        float d1 = i1.max - i0.min;

        depth = (d0 < d1) ? -d0 : d1;

        return !(i0.min > i1.max || i1.min > i0.max);
    }
};

#endif	/* INTERVAL_HPP */

