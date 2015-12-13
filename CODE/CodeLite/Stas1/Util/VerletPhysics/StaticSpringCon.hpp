/* 
 * File:   StaticSpringCon.hpp
 * Author: Stas
 *
 * Created on February 4, 2013, 8:22 AM
 */

#ifndef STATICSPRINGCON_HPP
#define	STATICSPRINGCON_HPP

#include "Particle.hpp"
#include "Constraint.hpp"

namespace VerletPhysics {

    struct StaticSpringCon : public Constraint {
        Particle* p;
        Vec2f v;
        float stiffness;

        void init(Particle* p, Vec2f const& v, float stiffness)
        {
            this->p = p;
            this->v = v;
            this->stiffness = stiffness;
        }

        StaticSpringCon(Particle* p, Vec2f const& v, float stiffness)
        {
            init(p, v, stiffness);
        }

        void apply()
        {
            Particle::spring(*p, v, stiffness);
        }
    };

}

#endif	/* STATICSPRINGCON_HPP */

