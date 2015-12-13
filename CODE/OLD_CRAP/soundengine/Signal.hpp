#ifndef _Signal_hpp_
#define _Signal_hpp_

#include <allegro.h>
#include "boost/shared_ptr.hpp"
#include "Types.hpp"

#include "MissingFileException.hpp"

#define CALL_METHOD(object, methodPtr)  ((object).*(methodPtr))

namespace Audio {

class Signal {
    bool mStereo;
    int mFreq;
    unsigned long mLen;
    uint16 *mLeftData, *mRightData;
    uint16 mDc;
    
    static uint16 getSample(SAMPLE* s, unsigned long i);
    void computeDc();
    void fromAllegroSample(SAMPLE* s, bool spatial);

public:
    class Handle;
    typedef boost::shared_ptr<Signal> Sptr;

    Signal();
    Signal(SAMPLE* s, bool spatial = true);
    Signal(char const* path, bool spatial = true);
    ~Signal();
};

class Signal::Handle {
    Signal::Sptr mSignal;
    
    uint16 _sample(uint16 const*, int pos) const;
    uint16 _sample(uint16 const*, double pos) const;
    
    uint16 _loopedSample(uint16 const*, int pos) const;
    uint16 _loopedSample(uint16 const*, double pos) const;
    
public:
    typedef uint16 (Signal::Handle::*getSampi)(int pos) const;
    typedef uint16 (Signal::Handle::*getSampf)(double pos) const;

    Handle();
    Handle(Signal* signal);
    Handle(Signal::Sptr sptr);
    
    Signal::Sptr getSptr();
    
    bool operator==(Handle const& other) const;
    int freq() const;
    
    bool stereo() const;
    unsigned long len() const;
    uint16 const* data() const;
    uint16 const* leftData() const;
    uint16 const* rightData() const;
    uint16 dc() const;
    long clampPos(long pos) const;
    
    uint16 sample(int pos) const;
    uint16 sample(double pos) const;
    
    uint16 loopedSample(int pos) const;
    uint16 loopedSample(double pos) const;
    
    uint16 sampleLeft(int pos) const;
    uint16 sampleLeft(double pos) const;
    
    uint16 loopedSampleLeft(int pos) const;
    uint16 loopedSampleLeft(double pos) const;
    
    uint16 sampleRight(int pos) const;
    uint16 sampleRight(double pos) const;
    
    uint16 loopedSampleRight(int pos) const;
    uint16 loopedSampleRight(double pos) const;
};

}

#endif
