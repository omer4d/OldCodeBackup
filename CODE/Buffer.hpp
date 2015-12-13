/* 
 * File:   Buffer.hpp
 * Author: Stas
 *
 * Created on June 23, 2012, 11:21 PM
 */

#ifndef BUFFER_HPP
#define	BUFFER_HPP

#include "Util.hpp"

template <class T>
class Buffer {
    int width, height;
    T* data;
    
public:
    void reset(int len)
    {
        if(data)
            delete[] data;
            
        this->width = len;
        this->height = 1;
        this->data = new T[len];
    }

    void reset(int w, int h)
    {
        if(data)
            delete[] data;
        
        this->width = w;
        this->height = h;
        this->data = new T[w * h];
    }
    
    Buffer()
    {
        this->width = 0;
        this->height = 0;
        this->data = NULL;
    }
    
    Buffer(int len)
    {
        this->data = NULL;
        reset(len);
    }
    
    Buffer(int w, int h)
    {
        this->data = NULL;
        reset(w, h);
    }
    
    ~Buffer()
    {
        delete[] data;
    }

    void fill(T const& t)
    {
        for(int i = 0; i < width * height; i++)
            data[i] = t;
    }
    
    // ****************
    // * Get a sample *
    // ****************
    
    T const& at(int n) const
    {
        return data[n];
    }

    T& at(int n)
    {
        return data[n];
    }

    T const& at(int x, int y) const
    {
        return data[y * width + x];
    }

    T& at(int x, int y)
    {
        return data[y * width + x];
    }
    
    // ********************************
    // * Get a sample with wraparound *
    // ********************************
    
    T const& wat(int n) const
    {
        return data[Util::wrapi(n, 0, width * height)];
    }

    T& wat(int n)
    {
        return data[Util::wrapi(n, 0, width * height)];
    }

    T const& wat(int x, int y) const
    {
        return data[Util::wrapi(y, 0, height) * width + Util::wrapi(x, 0, width)];
    }

    T& wat(int x, int y)
    {
        return data[Util::wrapi(y, 0, height) * width + Util::wrapi(x, 0, width)];
    }
    
    // ***************
    // * Other stuff *
    // ***************
    
    T const* getRawData() const
    {
        return data;
    }
    
    T* getRawData()
    {
        return data;
    }

    int getHeight() const
    {
        return height;
    }

    int getWidth() const
    {
        return width;
    }
    
    int getLen() const
    {
        return width * height;
    }
    
    int clampX(int x) const
    {
        if(x < 0) return 0;
        else if(x > width - 1) return width - 1;
        else return x;
    }
    
    int clampY(int y) const
    {
        if(y < 0) return 0;
        else if(y > height - 1) return height - 1;
        else return y;
    }
};

#endif	/* BUFFER_HPP */

