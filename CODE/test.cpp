#include <stdio.h>
#include <conio.h>

#include <string>
#include <sstream>

    static int parseColor(std::string const& code, int def)
    {
        std::stringstream converter;
        int res;

        if(code[0] == '#')
        {
            converter << &code[1];
            converter >> std::hex >> res;
        }
        
        else
        {
            converter << &code;
            converter >> res;
        }
        
        return !converter.fail() ? res : def;
    }

int main()
{
   std::string str = "   111   222   333";
   std::stringstream stream;
   
   stream << str;
   
   int a;
   std::string temp;
   
   stream >> temp;
   
   printf("%s", temp.c_str());
   
   getch();
 
   return 0;
}
