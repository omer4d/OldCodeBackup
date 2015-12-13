#ifndef _HRTF_hpp_
#define _HRTF_hpp_

#include <fftw3.h>
#include <allegro.h>
#include "Complex.hpp"
#include "Types.hpp"

namespace Audio {

class HRTF {
    Complex* mLeft;
    Complex* mRight;
    int mFirLen, mLogicLen, mRealLen;
    double mLeftILD, mRightILD;

    static int nextPow2(int n);

public:

    void init(int pFirLen, int buffLen);

    HRTF();
    HRTF(int pFirLen, int buffLen);

    void processFir(SAMPLE* samp, int buffLen);
    void blend(HRTF const& a, HRTF const& b, double k);
    void crossfade(HRTF const& a, HRTF const& b);
    static void blendILD(HRTF const& a, HRTF const& b, double k, double& left, double& right);
    Complex const* left() const;
    Complex const* right() const;
    int realLen() const;
    int logicLen() const;
    int firLen() const;

    ~HRTF();
};

}

#endif
