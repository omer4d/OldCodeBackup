#ifndef _StaticStickCon_hpp_
#define _StaticStickCon_hpp_

#include "Constraint.hpp"
#include "Particle.hpp"

namespace VerletPhysics {

struct StaticStickCon: public Constraint {
       Particle* p;
       Vec2f v;
       float restLen, stiffness;
       
       void init(Particle* p, Vec2f const& v, float restLen, float stiffness)
       {
           this->p = p;
           this->v = v;
           this->restLen = restLen;
           this->stiffness = stiffness;
       }
       
       StaticStickCon(Particle* p, Vec2f const& v, float restLen, float stiffness)
       {
           init(p, v, restLen, stiffness);
       }
       
       StaticStickCon(Particle* p, Vec2f const& v, float stiffness)
       {
           init(p, v, (p->pos - v).length(), stiffness);
       }
       
       void apply()
       {
            Particle::stick(*p, v, restLen, stiffness);
       }
};

}

#endif
