#include "Signal.hpp"

namespace Audio {

    uint16 Signal::getSample(SAMPLE* s, unsigned long i)
    {
        if(s->bits == 8) return uint16(((unsigned char *) s->data)[i] / 255.0 * 65535.0);
        else return((uint16 *) s->data)[i];
    }

    void Signal::computeDc()
    {
        double globalAvg = 0.0;

        for(int j = 0; j < mLen / 512; j++)
        {
            double localAvg = 0.0;
            for(int i = 0; i < 512; i++)
                localAvg += mLeftData[j * 512 + i];

            globalAvg += localAvg / 512.0;
        }

        globalAvg /= mLen / 512.0;
        mDc = uint16(globalAvg);
    }

    void Signal::fromAllegroSample(SAMPLE* s, bool spatial)
    {
        int n;
        mFreq = s->freq;
        mLen = s->len;
        mStereo = s->stereo != 0;
        
        if(s->stereo)
        {
            mLeftData = new uint16[mLen];
            mRightData = new uint16[mLen];
            
            for(int i = 0; i < s->len; i++)
            {
                mLeftData[i] = getSample(s, i * 2);
                mRightData[i] = getSample(s, i * 2 + 1);
            }
        }
        
        else
        {
            mLeftData = new uint16[mLen];
            mRightData = NULL;
            
            for(int i = 0; i < s->len; i++)
                mLeftData[i] = getSample(s, i);
        }
        
        computeDc();
    }
    
    Signal::Signal()
    {
        mStereo = true;
        mFreq = 44100;
        mLen = 512;
        mLeftData = new uint16[mLen * 2];
        
        for(int i = 0; i < mLen * 2; i++)
            mLeftData[i] = 32767;
        
        computeDc();
    }

    Signal::Signal(SAMPLE* s, bool spatial)
    {
        fromAllegroSample(s, spatial);
    }

    Signal::Signal(char const* path, bool spatial)
    {
        SAMPLE* s = load_sample(path);
        
        if(!s) throw MissingFileException(path);
        
        fromAllegroSample(s, spatial);
        destroy_sample(s);
    }

    Signal::~Signal()
    {
        delete[] mLeftData;
    }
    
    // *****************
    // * Signal Handle *
    // *****************
    
    Signal::Handle::Handle(Signal::Sptr sptr) : mSignal(sptr)
    {
    }
    
    Signal::Handle::Handle() : mSignal(new Signal())
    {
    }

    Signal::Handle::Handle(Signal* signal) : mSignal(signal)
    {
    }
    
    // ***********
    // * Private *
    // ***********
    
    uint16 Signal::Handle::_sample(uint16 const* data, int pos) const
    {
        if(pos < 0) return data[0];
        else if(pos > mSignal->mLen - 1) return data[mSignal->mLen - 1];

        return data[pos];
    }

    uint16 Signal::Handle::_sample(uint16 const* data, double pos) const
    {
        if(pos < 0) return data[0];
        else if(pos > mSignal->mLen - 1) return data[mSignal->mLen - 1];

        long posi = long(pos);
        double k = pos - posi;
        return uint16(data[posi] * (1.0 - k) + data[posi + 1] * k);
    }

    uint16 Signal::Handle::_loopedSample(uint16 const* data, int pos) const
    {
        if(pos < 0)
            pos += -mSignal->mLen * (pos / mSignal->mLen - 1);
        pos %= mSignal->mLen;

        return data[pos];
    }

    uint16 Signal::Handle::_loopedSample(uint16 const* data, double pos) const
    {
        if(pos < 0)
            pos += -mSignal->mLen * int(pos / mSignal->mLen - 1);

        long posi = long(pos);
        double k = pos - posi;
        posi %= mSignal->mLen;

        return uint16(data[posi] * (1.0 - k) + data[(posi + 1) % mSignal->mLen] * k);
    }
    
    // **********
    // * Public *
    // **********
    
    Signal::Sptr Signal::Handle::getSptr()
    {
        return mSignal;
    }
    
    bool Signal::Handle::operator==(Handle const& other) const
    {
        return mSignal == other.mSignal;
    }

    bool Signal::Handle::stereo() const
    {
        return mSignal->mStereo;
    }

    int Signal::Handle::freq() const
    {
        return mSignal->mFreq;
    }

    unsigned long Signal::Handle::len() const
    {
        return mSignal->mLen;
    }

    uint16 const* Signal::Handle::data() const
    {
        return mSignal->mLeftData;
    }

    uint16 Signal::Handle::dc() const
    {
        return mSignal->mDc;
    }

    long Signal::Handle::clampPos(long pos) const
    {
        if(pos < 0) return 0;
        else if(pos > mSignal->mLen - 1) return mSignal->mLen - 1;
        else return pos;
    }

    // ********
    // * Mono *
    // ********
    
    uint16 Signal::Handle::sample(int pos) const
    {
        return _sample(mSignal->mLeftData, pos);
    }

    uint16 Signal::Handle::sample(double pos) const
    {
        return _sample(mSignal->mLeftData, pos);
    }

    uint16 Signal::Handle::loopedSample(int pos) const
    {
        return _loopedSample(mSignal->mLeftData, pos);
    }
    
    uint16 Signal::Handle::loopedSample(double pos) const
    {
        return _loopedSample(mSignal->mLeftData, pos);
    }
    
    // ********
    // * Left *
    // ********
    
    uint16 Signal::Handle::sampleLeft(int pos) const
    {
        return _sample(mSignal->mLeftData, pos);
    }

    uint16 Signal::Handle::sampleLeft(double pos) const
    {
        return _sample(mSignal->mLeftData, pos);
    }

    uint16 Signal::Handle::loopedSampleLeft(int pos) const
    {
        return _loopedSample(mSignal->mLeftData, pos);
    }
    
    uint16 Signal::Handle::loopedSampleLeft(double pos) const
    {
        return _loopedSample(mSignal->mLeftData, pos);
    }
    
    // *********
    // * Right *
    // *********

    uint16 Signal::Handle::sampleRight(int pos) const
    {
        return _sample(mSignal->mRightData, pos);
    }

    uint16 Signal::Handle::sampleRight(double pos) const
    {
        return _sample(mSignal->mRightData, pos);
    }

    uint16 Signal::Handle::loopedSampleRight(int pos) const
    {
        return _loopedSample(mSignal->mRightData, pos);
    }

    uint16 Signal::Handle::loopedSampleRight(double pos) const
    {
        return _loopedSample(mSignal->mRightData, pos);
    }
}
