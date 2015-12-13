/* 
 * File:   Readable.hpp
 * Author: Stas
 *
 * Created on February 21, 2013, 9:24 AM
 */

#ifndef READABLE_HPP
#define	READABLE_HPP

struct Readable {
       virtual size_t read(void* dest, size_t bytes);
};

#endif	/* READABLE_HPP */

