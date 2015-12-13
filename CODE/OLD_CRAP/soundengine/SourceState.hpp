#ifndef _SourceState_hpp_
#define _SourceState_hpp_

namespace Audio {

struct SourceState {
 double leftShift, rightShift, intens, relYaw;

 void takeStep(SourceState const& step)
  {
   leftShift += step.leftShift;
   rightShift += step.rightShift;
   intens += step.intens;
  }

 static SourceState calcStep(SourceState const& a, SourceState const& b, int n)
  {
   SourceState c;

   c.leftShift = (b.leftShift - a.leftShift) / n;
   c.rightShift = (b.rightShift - a.rightShift) / n;
   c.intens = (b.intens - a.intens) / n;

   return c;
  }
};

}

#endif
