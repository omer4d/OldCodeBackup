/* 
 * File:   Buffer.hpp
 * Author: Stas
 *
 * Created on June 23, 2012, 11:21 PM
 */

#ifndef BUFFER_HPP
#define	BUFFER_HPP

template <class T>
class Buffer {
    int width, height;
    T* data;
    
    static int wrapi(int n, int min, int max)
    {
           int t = n - min, l = max - min;
           int mod = t % l;
           return mod >= 0 ? min + mod : min + l + mod;
    }
    
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
        return data[wrapi(n, 0, width * height)];
    }

    T& wat(int n)
    {
        return data[wrapi(n, 0, width * height)];
    }

    T const& wat(int x, int y) const
    {
        return data[wrapi(y, 0, height) * width + wrapi(x, 0, width)];
    }

    T& wat(int x, int y)
    {
        return data[wrapi(y, 0, height) * width + wrapi(x, 0, width)];
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
};

#endif	/* BUFFER_HPP */

