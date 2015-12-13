#ifndef _Buffer_hpp_
#define _Buffer_hpp_

template <class T>
struct Buffer {
      int len;
      T* data;
      
      void clear(T const& t)
      {
       for(int i = 0; i < len; i++)
        data[i] = t;
      }
      
      void copy(Buffer<T> const& other)
      {
       for(int i = 0; i < MIN(this->len, other.len); i++)
        this->data[i] = other.data[i];
      }
      
      Buffer(int len)
      {
       this->len = len;
       data = new T[len];
      }
      
      ~Buffer()
      {
        delete[] data;
      }
      
      T& at(int i)
      {
       return data[i];
      }
};

#endif
