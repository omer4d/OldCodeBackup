/* 
 * File:   Writable.hpp
 * Author: Stas
 *
 * Created on February 21, 2013, 9:24 AM
 */

#ifndef WRITABLE_HPP
#define	WRITABLE_HPP

struct Writable {
       virtual size_t write(void const* src, size_t bytes);
};

#endif	/* WRITABLE_HPP */

