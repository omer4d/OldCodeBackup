/* 
 * File:   Buffer.hpp
 * Author: Stas
 *
 * Created on June 23, 2012, 11:21 PM
 */

#ifndef BUFFER_HPP
#define	BUFFER_HPP

#include "Util.hpp"
#include <cstring>
#include <utility>
#include <type_traits>

template <class T>
class Buffer {
    int width, height;
    T* data;
    
public:
    void reset(int len)
    {
		delete[] data;
            
        this->width = len;
        this->height = 1;
        this->data = new T[len];
    }

    void reset(int w, int h)
    {
		delete[] data;
        
        this->width = w;
        this->height = h;
        this->data = new T[w * h];
    }
    
    Buffer()
    {
        this->width = 0;
        this->height = 0;
        this->data = nullptr;
    }
    
    Buffer(int len)
    {
        this->data = nullptr;
        reset(len);
    }
    
    Buffer(int w, int h)
    {
        this->data = nullptr;
        reset(w, h);
    }
	
	Buffer(Buffer const& other)
	{
		this->data = nullptr;
		reset(other.width, other.height);
		
		if(std::is_trivial<T>::value)
			memcpy(this->data, other.data, sizeof(T) * other.width * other.height);
		else
			for(int i = 0; i < width * height; ++i)
				data[i] = other.data[i];
	}
	
	Buffer(Buffer&& other)
	{
		this->data = other.data;
		this->width = other.width;
		this->height = other.height;
		other.data = nullptr;
	}
    
    ~Buffer()
    {
        delete[] data;
    }

	Buffer& operator=(Buffer&& other)
	{
		auto tmp = this->data;
		this->data = other.data;
		this->width = other.width;
		this->height = other.height;
		other.data = tmp;
		return *this;
	}

	Buffer& operator=(Buffer const& other)
	{
		Buffer tmp(other);
		*this = std::move(tmp);
		return *this;
	}

    void fill(T const& t)
    {
        for(int i = 0; i < width * height; i++)
            data[i] = t;
    }

    void put(Buffer const& src, int dx0 = 0, int dy0 = 0,
             int sx0 = 0, int sy0 = 0, int sx1 = -1, int sy1 = -1)
	{
		// Performs bounds checking only on the destination!
		
        if(sx1 < 0) sx1 += src.getWidth();
        if(sy1 < 0) sy1 += src.getHeight();

        int dx1 = dx0 + sx1 - sx0;
        int dy1 = dy0 + sy1 - sy0;

        int tx = sx0 - dx0;
        int ty = sy0 - dy0;

        if(dx1 < 0 || dy1 < 0 || dx0 > width - 1 || dy0 > height - 1)
            return;

        dx0 = clampX(dx0);
        dx1 = clampX(dx1);
        dy0 = clampY(dy0);
        dy1 = clampY(dy1);

        for(int y = dy0; y <= dy1; ++y)
            for(int x = dx0; x <= dx1; ++x)
                at(x, y) = src.at(x + tx, y + ty);
    }
    
    // ****************
    // * Get a sample *
    // ****************
    
    T const& operator[](int n) const
    {
        return data[n];
    }
    
    T& operator[](int n)
    {
        return data[n];
    }
    
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
    
    // ********************************
    // * Get sample with bounds check *
    // ********************************
    
    T sat(int n) const
    {
        if(n < 0 || n >= width * height)
            return T();
        else
            return data[n];
    }
    
    T sat(int x, int y) const
    {
        if(x < 0 || y < 0 || x >= width || y >= height)
            return T();
        else
            return data[y * width + x];
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
    
    bool contains(int x, int y) const
    {
        return x >= 0 && y >= 0 && x < width && y < height;
    }
};

#endif	/* BUFFER_HPP */

