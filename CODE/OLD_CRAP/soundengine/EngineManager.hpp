/* 
 * File:   EngineManager.hpp
 * Author: Stas
 *
 * Created on January 23, 2012, 12:46 PM
 */

#ifndef ENGINEMANAGER_HPP
#define	ENGINEMANAGER_HPP

#include <omp.h>

#include <allegro.h>
#include "Engine.hpp"
#include "Signal.hpp"
#include "Item.hpp"

#include "Logger.hpp"

namespace Audio {

    class EngineManager {
        Engine** mEngineArr;
        int mEngineNum;
        AUDIOSTREAM* mStream;
        int mBuffLen;
        HRTFDB* mHrtfdb;
        Logger mLogger;
        
    public:
        EngineManager(int pBuffLen);
        ~EngineManager();

        void setDefaultEnv(Env::Handle const& env);

        void resetSpatializer(int id);

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

        void setHeadState(Vec3f const& pos, double const& yaw, double const& pitch);
        Vec3f getHeadPos() const;
        double getHeadYaw() const;
        double getHeadPitch() const;
        double time() const;
        bool logic();

        void visualize(BITMAP* buffer) const;
    };

}

#endif	/* ENGINEMANAGER_HPP */

