/* 
 * File:   NoiseBase.hpp
 * Author: Stas
 *
 * Created on January 31, 2013, 8:05 AM
 */

#ifndef NOISEBASE_HPP
#define	NOISEBASE_HPP

class Noise1D {
public:
    virtual float at(float x) const = 0;
};

class Noise2D {
public:
    virtual float at(float x, float y) const = 0;
};

class Noise3D {
public:
    virtual float at(float x, float y, float z) const = 0;
};

#endif	/* NOISEBASE_HPP */

