#ifndef _Engine_hpp_
#define _Engine_hpp_

#include <list>

#include <allegro.h>

#include "Vec2f.hpp"
#include "Vec3f.hpp"

#include "Types.hpp"

#include "Signal.hpp"
#include "Item.hpp"
#include "Spatializer.hpp"
#include "HeadState.hpp"
#include "Env.hpp"

#include "Event.hpp"
#include "SourceEvent.hpp"
#include "Proxy.hpp"

namespace Audio {
    class Engine {
        typedef std::list<Item*> ItemList;
        
        long mWorkload;
        unsigned long mBuffLen, mOutFreq, mCycleNum;
        int16* mBuff;
        ItemList mPlayList;
        Spatializer* mSpatializer;
        Env::Handle mDefaultEnv;

        HeadState mHeadState;

        void spatialized(Item* event);
        void cheap(Item* event);
        void ambient(Item* event);

    public:
        Engine(HRTFDB& hrtfdb, int pBuffLen);
        ~Engine();

        int16 const* getBuffer() const;
        void resetSpatializer(HRTFDB& hrtfdb);

        void setDefaultEnv(Env::Handle const& env);

        Item::Handle play(Env::Handle const& env, Signal::Handle const& signal, int id,
                          Quality quality = FULL_QUALITY, double intens = 0.5, bool looped = false,
                          Vec3f const& pos = Vec3f());

        Item::Handle play(Signal::Handle const& signal, int id, Quality quality = FULL_QUALITY,
                          double intens = 0.5, bool looped = false, Vec3f const& pos = Vec3f());
        
        void stop(Signal::Handle const& signal);
        void stop(Signal::Handle const& signal, int id);
        void stopAll();
        void stopAll(int id);
        
        void addEventListener(ListenerProxy p, int id);
        void removeEventListener(ListenerProxy p, int id);

        // In radians:
        void setHeadState(Vec3f const& pos, double const& yaw, double const& pitch);
        Vec3f getHeadPos() const;
        double getHeadYaw() const;
        double getHeadPitch() const;

        void logic();

        double time() const;
        long workload() const;
        void visualize(BITMAP* buffer) const;
    };

}

#endif
