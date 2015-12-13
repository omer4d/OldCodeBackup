#ifndef _Env_hpp_
#define _Env_hpp_

#include "boost/shared_ptr.hpp"

namespace Audio {

    class Env {
        double mDecay, mSoundSpeed, mHeadRad;
        
    public:
        typedef boost::shared_ptr<Env> Sptr;
        class Handle;

        Env(double pDecay = 1.0, double pSoundSpeed=340.0, double pHeadRad=0.1)
        {
            mDecay = pDecay;
            mSoundSpeed = pSoundSpeed;
            mHeadRad = pHeadRad;
        }
    };

    class Env::Handle {
        Env::Sptr mEnv;

    public:

        Handle(Env* env) : mEnv(env)
        {
        }
        
        double getSoundSpeed() const { return mEnv->mSoundSpeed; }
        double getHeadRad() const { return mEnv->mHeadRad; }

        double decay(double i, Vec3f const& delta) const
        {
            double d = delta.dot(delta);
            if(d < 0.0001) return i;
            
            double k = 1.0 / (mEnv->mDecay * delta.dot(delta));
            if(k > 1.0) k = 1.0;
            return i * k;
        }
    };

}

#endif
