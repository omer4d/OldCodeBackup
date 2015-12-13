#ifndef _Spatializer_hpp_
#define _Spatializer_hpp_

#include <fftw3.h>

#include "HRTFDB.hpp"
#include "CircularArray.hpp"
#include "Complex.hpp"
#include "Types.hpp"

namespace Audio {

class Spatializer {
    int mBuffLen;
    HRTFDB& mHrtfdb;
    CircularArray mLeftTail, mRightTail;

    double* mLeftSigTime;
    double* mRightSigTime;
    Complex* mSigFreq;
    Complex* mResFreq;

    fftw_plan mPlanForward[2], mPlanBackward[2];

    enum {
        RIGHT_CHAN, LEFT_CHAN
    };

public:

    Spatializer(HRTFDB& hrtfdb, int buffLen);
    ~Spatializer();
    
    void setHrtfdb(HRTFDB& hrtfdb);
    
    double* left();
    double* right();
    
    void renderTail(int16* buff);
    
    void ild(double yaw, double pitch, double& left, double& right);
    void render(int16* buff, double yaw, double pitch);
};

}

#endif
