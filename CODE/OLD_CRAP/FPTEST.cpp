#include <stdio.h>
#include <conio.h>
#include <inttypes.h>

#define INT32_MAX 2147483647

int main()
{
   int i = 0;
   float f = 0;
   
   while(i == int(f + 0.5f) && i < INT32_MAX)
   {
      ++i;
      ++f;
      
      //if(!(i % 100000))
        //printf("%d\n", i);
   }
 
   printf("Done: %d - %d", i, int(f + 0.5f));
 
   getch();
   return 0;
}
