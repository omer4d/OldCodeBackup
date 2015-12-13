#ifndef _Mat9f_hpp_
#define _Mat9f_hpp_

#include <cmath>

struct Mat9f {
    float m[3][3];
    
    static const Mat9f identity;
    
    float& operator()(int i, int j);
    float const& operator()(int i, int j) const;

    void initIdentity();
    void initScale(float sx, float sy);
    void initTranslation(float tx, float ty);
    void initRotation(float a);

    Mat9f();

    Mat9f operator*(Mat9f const& other) const;
    Mat9f& operator*=(Mat9f const& other);
    
    Mat9f& scale(float sx, float sy);
    Mat9f& translate(float tx, float ty);
    Mat9f& rotate(float a);
    Mat9f& invert();
};

#endif
