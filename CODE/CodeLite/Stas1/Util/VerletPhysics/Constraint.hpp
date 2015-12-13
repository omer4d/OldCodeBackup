#ifndef _CONSTRAINT_HPP_
#define _CONSTRAINT_HPP_

namespace VerletPhysics {

struct Constraint {
       virtual void apply() = 0;
};

}

#endif
