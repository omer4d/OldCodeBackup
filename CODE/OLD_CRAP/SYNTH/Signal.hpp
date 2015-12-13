#ifndef _Signal_hpp_
#define _Signal_hpp_

class Signal {
public:
   virtual float sample(double time) = 0;
   
   virtual ~Signal()
   {
   }
};

#endif
