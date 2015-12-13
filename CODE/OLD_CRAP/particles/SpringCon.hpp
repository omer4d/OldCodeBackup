#ifndef _SPRINGCON_HPP_
#define _SPRINGCON_HPP_

#include "Particle.hpp"
#include "Constraint.hpp"

struct SpringCon: public Constraint {
       Particle* p1;
       Particle* p2;
       float stiffness;
       
       void init(Particle* p1, Particle* p2, float stiffness)
       {
           this->p1 = p1;
           this->p2 = p2;
           this->stiffness = stiffness;
       }
       
       SpringCon(Particle* p1, Particle* p2, float stiffness)
       {
           init(p1, p2, stiffness);
       }
       
       void apply()
       {
            Particle::spring(*p1, *p2, stiffness);
       }
};

#endif
