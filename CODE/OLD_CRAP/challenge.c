#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define NULL_PTR_ERROR -1
#define MALLOC_ERROR   -2

// 1. Fixed str_cat not handling the following use case:
//    char *s = NULL;
//    str_cat(&s, "aaaa");

// 2. The new implementations make use of strcpy and strcat from string.h.
// 3. Return codes for malloc failure and bad arguments are irrelevant.
//    The new implementations return pointers to the results. This is useful for chaining calls.   
// 4. Redundant sizeof(char) removed. The C standard guarantees that sizeof(char) = 1.
// 5. Code style fixed to match main.
// 6. Out of memory errors should be handled on a high level.
//    Error codes are inappropriate since they would need to be propogated all the way up.
// 7. Arguments should be validated by the user on a higher level depending on their needs.
//    Bad str_* arguments are programming errors and should be left for the debugger to catch.
//    A good low-level C library validates arguments when not doing so may break the internal state of the lib
//    or expose vulnerabilities.
// 8. 

char **str_cpy(char **dest, char const *src)
{
     char *oldDest = *dest;
     *dest = strdup(src);
     free(oldDest);
     return dest;
}

char **str_cat(char **dest, char const *src)
{
    int n1 = *dest?strlen(*dest):0, n2 = strlen(src);
    char *oldDest = *dest;
    *dest = malloc(n1+n2+1);
    memcpy(*dest, oldDest, n1);
    memcpy(*dest+n1, src, n2+1);
    free(oldDest);
    return dest;
}

int main(int argc, char *argv[])
{
    char *s = NULL;
    
    //str_cat(&s, "zasd");
    str_cat(str_cat(str_cpy(&s, "asd"), "ZUZU"), "lala");

    puts(s);
    
    getch();
    return 0;
}
