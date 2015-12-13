#ifndef _Module_hpp_
#define _Module_hpp_

#include "Signal.hpp"
#include <vector>

class Module: public Signal {
protected:
      std::vector<Signal*> input;
      static int globalCycle;
      int cycle;
      
public:
       static void globalTick()
       {
          globalCycle++;
       }
       
       Module(int n): input(n)
       {
       }
       
       void setInput(int i, Signal* signal)
       {
            if(i < 0 || i > input.size() - 1)
                 throw std::runtime_error("Invalid input port!");
            input[i] = signal;
       }
       
       virtual void onTick(double dt) = 0;
       
       virtual void tick(double dt)
       {
            if(cycle >= globalCycle)
               return;
            
            for(int i = 0; i < input.size(); i++)
               if(input[i])
                 input[i]->tick(dt);
            onTick(dt);
            cycle++;
       }
       
       virtual void onNoteOn(int n)
       {
       }
       
       virtual void onNoteOff(int n)
       {
       }
};

int Module::globalCycle = 0;

#endif
