#ifndef _HeadState_hpp_
#define _HeadState_hpp_

#include "Vec3f.hpp"

namespace Audio {

struct HeadState {
    Vec3f pos;
    double yaw, pitch;
    
    HeadState()
    {
        yaw = 0.0;
        pitch = 0.0;
    }
};

}

#endif
