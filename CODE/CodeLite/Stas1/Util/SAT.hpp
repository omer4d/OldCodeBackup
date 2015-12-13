/* 
 * File:   SAT.hpp
 * Author: Stas
 *
 * Created on June 27, 2012, 9:00 AM
 */

#ifndef SAT_HPP
#define	SAT_HPP

#include <float.h>
#include <vector>

#include "Vec2f.hpp"
#include "Shape.hpp"
#include "Interval.hpp"

class SAT {
    std::vector<Vec2f> norms;
    static SAT* instance;
    
public:
    SAT()
    {
        norms.reserve(200);
    }
    
    static SAT& getInstance();
    
    void pushAxisNormal(Vec2f const& v);
    
    void clearAxisNormals();
    
    bool test(Shape const& shape1, Mat9f const& mat1, Shape const& shape2, Mat9f const& mat2);
    
    bool test(Shape const& shape1, Mat9f const& mat1,
                     Shape const& shape2, Mat9f const& mat2,
                     Vec2f &mtd, Vec2f& mtdNorm);
};

#endif	/* SAT_HPP */

