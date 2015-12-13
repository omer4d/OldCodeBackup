#ifndef _Signal_hpp_
#define _Signal_hpp_

class Signal {
public:
       
   virtual float sample() = 0;
   virtual void tick(double dt) = 0;
   
   virtual ~Signal()
   {
   }
};

#endif
