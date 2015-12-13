/* 
 * File:   SafePtr.hpp
 * Author: Stas
 *
 * Created on January 29, 2012, 5:15 PM
 */

#ifndef SAFEPTR_HPP
#define	SAFEPTR_HPP

#include <boost/shared_ptr.hpp>

template <class T>
class SafePtr : public boost::shared_ptr<T*> {
public:

    SafePtr() : boost::shared_ptr<T*>(new T*(NULL))
    {
    }

    SafePtr(T& ul) : boost::shared_ptr<T*>(new T*(&ul))
    {
    }

    SafePtr(T* ul) : boost::shared_ptr<T*>(new T*(ul))
    {
    }

    void nullify()
    {
        *this->get() = NULL;
    }

    T* operator ->()
    {
        return *this->get();
    }

    T& operator *()
    {
        return * * this->get();
    }

    T const* operator ->() const
    {
        return *this->get();
    }

    T const& operator *() const
    {
        return * * this->get();
    }

    T* getRawPtr()
    {
        return *this->get();
    }

    bool isNull() const
    {
        return *this->get() == NULL;
    }
};

#endif	/* SAFEPTR_HPP */