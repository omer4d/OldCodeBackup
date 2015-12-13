#include "HRTF.hpp"

namespace Audio {

int HRTF::nextPow2(int n)
{
    int i;
    for(i = 1; i < n; i *= 2);
    return i;
}

void HRTF::init(int pFirLen, int buffLen)
{
    mFirLen = pFirLen;
    mLogicLen = mFirLen + buffLen - 1;
    mRealLen = mLogicLen / 2 + 1;

    mLeft = fftw_alloc_complex(mRealLen);
    mRight = fftw_alloc_complex(mRealLen);
}

HRTF::HRTF()
{
    mLeft = NULL;
    mRight = NULL;
    mFirLen = 0;
    mLogicLen = 0;
    mRealLen = 0;
}

HRTF::HRTF(int pFirLen, int buffLen)
{
    init(pFirLen, buffLen);
}

void HRTF::processFir(SAMPLE* samp, int buffLen)
{
    double maxl = -1.0, maxr = -1.0;

    mFirLen = samp->len;
    mLogicLen = mFirLen + buffLen - 1;
    mRealLen = mLogicLen / 2 + 1;

    uint16* data = (uint16*) samp->data;

    double* fir = fftw_alloc_real(mLogicLen);
    mLeft = fftw_alloc_complex(mRealLen);
    mRight = fftw_alloc_complex(mRealLen);

    fftw_plan plan;

    plan = fftw_plan_dft_r2c_1d(mLogicLen, fir, mLeft, FFTW_MEASURE);

    for(int i = 0; i < mFirLen; i++)
    {
        fir[i] = s2f(data[i * 2 + 1]);
        if(fir[i] > maxl) maxl = fir[i];
    }
    for(int i = mFirLen; i < mLogicLen; i++) fir[i] = 0.0;

    fftw_execute(plan);
    fftw_destroy_plan(plan);

    plan = fftw_plan_dft_r2c_1d(mLogicLen, fir, mRight, FFTW_MEASURE);

    for(int i = 0; i < mFirLen; i++)
    {
        fir[i] = s2f(data[i * 2]);
        if(fir[i] > maxr) maxr = fir[i];
    }
    for(int i = mFirLen; i < mLogicLen; i++) fir[i] = 0.0;

    fftw_execute(plan);
    fftw_destroy_plan(plan);

    mLeftILD = (maxl + 1.0) * 0.5;
    mRightILD = (maxr + 1.0) * 0.5;

    fftw_free(fir);
}

void HRTF::blend(HRTF const& a, HRTF const& b, double k)
{
    int n = MIN(MIN(a.realLen(), b.realLen()), mRealLen);

    for(int i = 0; i < n; i++)
    {
        Complex_blend(mLeft[i], a.mLeft[i], b.mLeft[i], k);
        Complex_blend(mRight[i], a.mRight[i], b.mRight[i], k);
    }
}

void HRTF::crossfade(HRTF const& a, HRTF const& b)
{
    int n = MIN(MIN(a.realLen(), b.realLen()), mRealLen);

    for(int i = 0; i < n; i++)
    {
        Complex_blend(mLeft[i], a.mLeft[i], b.mLeft[i], double(i) / double(n - 1));
        Complex_blend(mRight[i], a.mRight[i], b.mRight[i], double(i) / double(n - 1));
    }
}

void HRTF::blendILD(HRTF const& a, HRTF const& b, double k, double& left, double& right)
{
    left = b.mLeftILD * k + a.mLeftILD * (1.0 - k);
    right = b.mRightILD * k + a.mRightILD * (1.0 - k);
}

Complex const* HRTF::left() const
{
    return mLeft;
}

Complex const* HRTF::right() const
{
    return mRight;
}

int HRTF::realLen() const
{
    return mRealLen;
}

int HRTF::logicLen() const
{
    return mLogicLen;
}

int HRTF::firLen() const
{
    return mFirLen;
}

HRTF::~HRTF()
{
    fftw_free(mLeft);
    fftw_free(mRight);
}

}