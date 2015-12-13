/* 
 * File:   CircularBuffer.hpp
 * Author: Stas
 *
 * Created on August 6, 2013, 10:24 AM
 */

#ifndef CIRCULARBUFFER_HPP
#define	CIRCULARBUFFER_HPP

template <class T>
class CircularBuffer {
    int n, start;
    T* data;

    int wrap(int i) const
    {
        int r = i % n;
        return r >= 0 ? r : r + n;
    }

public:
    int getSize() const
    {
        return n;
    }
    
    void init(int n, T defVal)
    {
        delete[] data;
        
        this->n = n;
        this->data = new T[n];
        this->start = 0;

        for(int i = 0; i < n; ++i)
            data[i] = defVal;
    }
    
    CircularBuffer()
    {
        n = 0;
        start = 0;
        data = (T*)0;
    }
    
    CircularBuffer(int n, T defVal)
    {
        init(n, defVal);
    }
    
    ~CircularBuffer()
    {
        delete[] data;
    }

    void rotate(int p)
    {
        start = wrap(start - p);
    }
    
    T const& operator[](int i) const
    {
        return data[wrap(start + i)];
    }

    T& operator[](int i)
    {
        return data[wrap(start + i)];
    }
    
    T const& front() const
    {
        return (*this)[0];
    }
    
    T& front()
    {
        return (*this)[0];
    }
    
    T const& back() const
    {
        return (*this)[n - 1];
    }
    
    T& back()
    {
        return (*this)[n - 1];
    }
};

#endif	/* CIRCULARBUFFER_HPP */

