#ifndef _DC_hpp_
#define _DC_hpp_

#include "Signal.hpp"

class DC: public Signal {
   float val;
   static DC zero;
   
public:
   static DC* getZero()
   {
      return &zero;
   }
   
   DC()
   {
       this->val = 0.f;
   }
   
   DC(float val)
   {
      this->val = val;
   }
   
   float sample()
   {
       return val;
   }
   
   void tick(double dt)
   {
   }
};

DC DC::zero(0.0);

#endif
