#include "Item.hpp"

namespace Audio {

    
    Item::Item(double pStep, Env::Handle const& pEnv, Signal::Handle const& pSignal, int pId,
                 Quality pQuality, double pIntens, bool pLooped, Vec3f const& pPos,
                 HeadState const& head) : signal(pSignal), env(pEnv), alive(new bool(true))
    {
        id = pId;
        quality = pQuality;
        looped = pLooped;
        curr = 0.0;
        step = pStep;
        pos = pPos;
        intens = pIntens;
        lastState = state(head);
    }
    
    Item::~Item()
    {
        *alive = false;
    }

    SourceState Item::state(HeadState const& head) const
    {
        SourceState s;

        if(quality == AMBIENT_QUALITY)
        {
            s.leftShift = 0.0;
            s.rightShift = 0.0;
            s.intens = intens;
            s.relYaw = 0.0;
        }

        else
        {
            double r = env.getHeadRad(), v = env.getSoundSpeed();

            Vec2f dir(cos(head.yaw), sin(head.yaw)), norm = dir.normal();
            Vec2f delta(pos.x - head.pos.x, pos.z - head.pos.z);

            Vec3f left(head.pos.x - r * norm.x, head.pos.y, head.pos.z - r * norm.y);
            Vec3f right(head.pos.x + r * norm.x, head.pos.y, head.pos.z + r * norm.y);

            s.leftShift = (pos - left).length() / v * signal.freq();
            s.rightShift = (pos - right).length() / v * signal.freq();
            s.intens = env.decay(intens, pos - head.pos);
            s.relYaw = Vec2f::ang(dir, delta, 1.0, delta.length());
        }

        return s;
    }

}