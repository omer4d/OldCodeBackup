#ifndef _Module_hpp_
#define _Module_hpp_

#include "Signal.hpp"
#include <vector>

class Module: public Signal {
protected:
      std::vector<Signal*> input;
      
public:
       Module()
       {
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
};

#endif
