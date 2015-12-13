#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef void FAILURE_CALLBACK(void);

FAILURE_CALLBACK *failure_cb = NULL; // Gives the user a chance to free resources and retry or save data.

void set_failure_callback(FAILURE_CALLBACK *fcb)
{
    failure_cb = fcb;
}

// *****************************************************************************
// * WARNING!                                                                  *
// * The user is responsible for deallocation AFTER the functions return!      *
// * We can't free or realloc dst because we don't know how it was allocated!  *
// * e.g.:                                                                     *
// * char *s = "TROLOLO";                                                      *
// * str_cat(&s, "LOLO");                                                      *
// *****************************************************************************

char *str_cpy(char **dst, const char *src)
{
    if(!(*dst = malloc(strlen(src)+1)) && failure_cb)
    {
        failure_cb();
        return *dst;
    }
    return strcpy(*dst, src);
}

char *str_cat(char **dst, const char *src)
{
    char *temp = *dst;
    if(!*dst)
        return str_cpy(dst, src);
    else if(!(*dst = malloc(strlen(*dst)+strlen(src)+1)) && failure_cb)
    {
        failure_cb();
        return *dst;
    }
    return strcat(strcpy(*dst, temp), src);
}

int main(int argc, char *argv[])
{
    char *s = NULL;
    char s2[1000];
    char* s3 = s2;
    
    //free(s);
    str_cpy(&s3, "troll");
    str_cpy(&s, "troll");
    str_cpy(&s, s+1);
    str_cat(&s, s);
   
    
    //str_cat(&s, "abcd");
    //str_cat(&s, NULL);
    
    //str_cpy(&s, s+4);
    
    puts(str_cat(&s, s+1));
    
    //str_cpy(&s, "Hola Hola");
    //str_cpy(&s, s+5);
    //str_cat(&s, " Mundo");
    //puts(s);
    
    //str_cpy(&s, s+5);
    //
    
    
    //str_printf(&s, "%s!", s);
    //puts(s); /* result: "Hola Mundo!" */
    //str_free(&s);
    
    getch();
    return 0;
}
