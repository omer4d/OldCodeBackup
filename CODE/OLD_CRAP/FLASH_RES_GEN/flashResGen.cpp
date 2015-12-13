#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <string>

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     //install_mouse();
     //install_keyboard();
     //set_color_depth(32);
     //set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     //srand(time(NULL));
}

void deinit()
{
     //destroy_bitmap(buffer);
}


#define ALL_ATTS  (FA_DIREC | FA_ARCH) 
#define MAXPATH 200

int fileCount = 0;

void walker(FILE* file, const char *path, const char *findme)
{
  struct al_ffblk  finder;
  unsigned int  res;

  for (res = al_findfirst("*.*", &finder, ALL_ATTS); res == 0; res = al_findnext(&finder))
  {
    if (strcmp(finder.name, ".") == 0) continue;
    if (strcmp(finder.name, "..") == 0) continue;
    
    if (finder.attrib & FA_DIREC)
    {
      char newpath[MAXPATH];
      strcpy(newpath, path);
      strcat(newpath, "/");
      strcat(newpath, finder.name);
      chdir(finder.name);
      walker(file, newpath, findme);
      
      chdir("..");
    }
    
    else if(finder.attrib & FA_ARCH)
    {
          char const* ext = get_extension(finder.name);
     
          if(strcmp(ext, "bmp") == 0 || strcmp(ext, "png") == 0)
          {
             std::string str = std::string(path) + "/" + finder.name;
           
             fprintf(file, "[Embed(source=\"..%s\")]\n", str.c_str());
             fprintf(file, "var file%d: Class;\n", fileCount);
             fprintf(file, "ResourceManager.instance.add(file%d, \"%s\");\n\n\n", fileCount, &str.c_str()[1]);
           
           fileCount++;
          }
          
          else if(strcmp(ext, "json") == 0)
          {
             std::string str = std::string(path) + "/" + finder.name;
           
             fprintf(file, "[Embed(source=\"..%s\", mimeType=\"application/octet-stream\")]\n", str.c_str());
             fprintf(file, "var file%d: Class;\n", fileCount);
             fprintf(file, "ResourceManager.instance.add(file%d, \"%s\");\n\n\n", fileCount, &str.c_str()[1]);
           
           fileCount++;
          }
    }
  }
}

int main()
{
    bool exit = false;
    
    init();
    
    char buff[2000];
    
    FILE* file = fopen("embed.txt", "w+");
    
    walker(file, "", buff);
    
    printf("%s", buff);
    
    /*
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
      
     clear_to_color(buffer, 0);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }*/
    
    deinit();
    return 0;
}END_OF_MAIN()
