#ifndef _PARTICLE_HPP_
#define _PARTICLE_HPP_

#include "Vec2f.hpp"

struct Particle {
    static float dt;
    static float damping;
    static Vec2f gravity;
    
    float invMass, rad;
    Vec2f oldPos, pos, force;
    
    Particle(Vec2f const& pos, float mass, float rad)
    {
        this->rad = rad;
        this->invMass = 1.f / mass;
        this->oldPos = pos;
        this->pos = pos;
        this->force.init(0, 0);
    }
    
    void tick()
    {
        Vec2f temp = pos;
        pos += (pos - oldPos) * 0.997 + (force * invMass + gravity) * dt * dt;
        force.init(0, 0);
        oldPos = temp;
    }
    
    static void constrain(Particle& p1, Particle& p2, float restLen, float stiffness)
    {
           Vec2f delta = p2.pos - p1.pos;
           float deltaLen = delta.length();
           float diff = (deltaLen - restLen) / (deltaLen * (p1.invMass + p2.invMass));
           
           p1.pos += delta * p1.invMass * diff * stiffness;
           p2.pos -= delta * p2.invMass * diff * stiffness;
           
    }
    
    static void constrain(Particle& p, Vec2f const& v, float restLen, float stiffness)
    {
           Vec2f delta = v - p.pos;
           float deltaLen = delta.length();
           float diff = (deltaLen - restLen) / deltaLen;
           p.pos += delta * diff;
    }
    
    static void spring(Particle& p1, Particle& p2, float stiffness)
    {
           Vec2f delta = p2.pos - p1.pos;
           p1.pos += delta * 0.5 * stiffness;
           p2.pos -= delta * 0.5 * stiffness;
    }
    
    static void spring(Particle& p, Vec2f const& v, float stiffness)
    {
           Vec2f delta = v - p.pos;
           p.pos += delta * stiffness;
    }
    
    void draw(BITMAP* buffer, int col)
    {
         DrawVec2f::circlefill(buffer, pos, int(rad), col);
    }
};

float Particle::dt = 0.01;
Vec2f Particle::gravity(0, 50);

#endif
