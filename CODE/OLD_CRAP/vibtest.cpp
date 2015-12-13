#include <stdio.h>
#include <conio.h>
#include <math.h>

struct Test {
       
       template <class T>
       virtual void func()
       {
          T* t = new T();
       }
};

int main()
{
   double k = pow(2, 1.0 / 12.0);
   double dt = 0.00001, t = 0.0, T = 0.0;
   
   while(t < 100)
   {
      t += dt;
      T += dt * pow(k, 12 * sin(2 * M_PI * 1 * t));
      printf("%f\n", T - t);
   }
   
   return 0;
}
