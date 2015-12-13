/* 
 * File:   ImageLoader.hpp
 * Author: Stas
 *
 * Created on February 21, 2013, 9:32 AM
 */

#ifndef IMAGELOADER_HPP
#define	IMAGELOADER_HPP

#include <inttypes.h>
#include "Buffer.hpp"
#include "Col32.hpp"

class ImageLoader {
public:
    static Buffer<int32_t>* load(char const* path);
};

#endif	/* IMAGELOADER_HPP */

