/* 
 * File:   Shape.hpp
 * Author: Stas
 *
 * Created on June 27, 2012, 8:55 AM
 */

#ifndef SHAPE_HPP
#define	SHAPE_HPP

#include "Vec2f.hpp"
#include "Mat9f.hpp"

class Interval;

class Shape {
public:
    virtual Interval project(Mat9f const& mat, Vec2f const& v) const = 0;
    virtual ~Shape()
    {
    }
};

#endif	/* SHAPE_HPP */

