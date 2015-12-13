#include "Spatializer.hpp"

namespace Audio {

Spatializer::Spatializer(HRTFDB& hrtfdb, int buffLen): mHrtfdb(hrtfdb)
{   
    mBuffLen = buffLen;
    mLeftTail.init(buffLen * 100);
    mRightTail.init(buffLen * 100);

    mLeftSigTime = fftw_alloc_real(mHrtfdb.logicLen());
    mRightSigTime = fftw_alloc_real(mHrtfdb.logicLen());

    mSigFreq = fftw_alloc_complex(mHrtfdb.realLen());
    mResFreq = fftw_alloc_complex(mHrtfdb.realLen());

    int logicLen = mHrtfdb.logicLen();

    mPlanForward[LEFT_CHAN] = fftw_plan_dft_r2c_1d(logicLen, mLeftSigTime, mSigFreq, FFTW_MEASURE);
    mPlanForward[RIGHT_CHAN] = fftw_plan_dft_r2c_1d(logicLen, mRightSigTime, mSigFreq, FFTW_MEASURE);

    mPlanBackward[LEFT_CHAN] = fftw_plan_dft_c2r_1d(logicLen, mResFreq, mLeftSigTime, FFTW_MEASURE);
    mPlanBackward[RIGHT_CHAN] = fftw_plan_dft_c2r_1d(logicLen, mResFreq, mRightSigTime, FFTW_MEASURE);
}

Spatializer::~Spatializer()
{
    fftw_destroy_plan(mPlanForward[0]);
    fftw_destroy_plan(mPlanBackward[0]);
    fftw_destroy_plan(mPlanForward[1]);
    fftw_destroy_plan(mPlanBackward[1]);

    fftw_free(mLeftSigTime);
    fftw_free(mRightSigTime);
    fftw_free(mSigFreq);
    fftw_free(mResFreq);
}

void Spatializer::setHrtfdb(HRTFDB& hrtfdb)
{
    if(mHrtfdb.realLen() == hrtfdb.realLen())
        mHrtfdb = hrtfdb;
    else
        printf("Audio Error: in setHrtfdb, new DB len doesn't match the original!\n");
}

double* Spatializer::left()
{
    return mLeftSigTime;
}

double* Spatializer::right()
{
    return mRightSigTime;
}

void Spatializer::renderTail(int16* buff)
{
    for(int i = 0; i < mBuffLen; i++)
    {
        buff[i * 2 + LEFT_CHAN] = mLeftTail[i];
        buff[i * 2 + RIGHT_CHAN] = mRightTail[i];
    }

    mLeftTail.rotate(-mBuffLen);
    mRightTail.rotate(-mBuffLen);
}

void Spatializer::ild(double yaw, double pitch, double& left, double& right)
{
    mHrtfdb.ild(yaw, pitch, left, right);
}

void Spatializer::render(int16* buff, double yaw, double pitch)
{
    HRTF const& hrtf = mHrtfdb.geti(yaw, pitch);
    int realLen = mHrtfdb.realLen(), logicLen = mHrtfdb.logicLen();
    Complex const* left = hrtf.left();
    Complex const* right = hrtf.right();

    // Zero padding:
    for(int i = mBuffLen; i < logicLen; i++)
    {
        mLeftSigTime[i] = 0.0;
        mRightSigTime[i] = 0.0;
    }

    // Left channel:
    fftw_execute(mPlanForward[LEFT_CHAN]);
    for(int i = 0; i < realLen; i++)
        Complex_mul(mResFreq[i], mSigFreq[i], left[i]);
    fftw_execute(mPlanBackward[LEFT_CHAN]);

    for(int i = 0; i < mBuffLen; i++)
        buff[i * 2 + LEFT_CHAN] += int16(mLeftSigTime[i] / logicLen);
    for(int i = mBuffLen; i < logicLen; i++)
        mLeftTail[i - mBuffLen] += int16(mLeftSigTime[i] / logicLen);

    // Right channel:
    fftw_execute(mPlanForward[RIGHT_CHAN]);
    for(int i = 0; i < realLen; i++)
        Complex_mul(mResFreq[i], mSigFreq[i], right[i]);
    fftw_execute(mPlanBackward[RIGHT_CHAN]);

    for(int i = 0; i < mBuffLen; i++)
        buff[i * 2 + RIGHT_CHAN] += int16(mRightSigTime[i] / logicLen);
    for(int i = mBuffLen; i < logicLen; i++)
        mRightTail[i - mBuffLen] += int16(mRightSigTime[i] / logicLen);
}

}