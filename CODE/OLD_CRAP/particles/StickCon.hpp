#ifndef _StickCon_hpp_
#define _StickCon_hpp_

#include "Constraint.hpp"
#include "Particle.hpp"

struct StickCon: public Constraint {
       Particle* p1;
       Particle* p2;
       float restLen, stiffness;
       
       void init(Particle* p1, Particle* p2, float restLen, float stiffness)
       {
           this->p1 = p1;
           this->p2 = p2;
           this->restLen = restLen;
           this->stiffness = stiffness;
       }
       
       StickCon(Particle* p1, Particle* p2, float restLen, float stiffness)
       {
           init(p1, p2, restLen, stiffness);
       }
       
       StickCon(Particle* p1, Particle* p2, float stiffness)
       {
           init(p1, p2, (p1->pos - p2->pos).length(), stiffness);
       }
       
       void apply()
       {
            Particle::constrain(*p1, *p2, restLen, stiffness);
       }
};


#endif
