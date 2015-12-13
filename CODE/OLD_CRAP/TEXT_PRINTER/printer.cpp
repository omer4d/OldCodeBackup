#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <allegro/internal/aintern.h>

#include "List.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]


BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

float minSpacing = 8.f;

struct Line {
   char const* start;
   char const* end;
   int wordNum;
   float wordWidthSum;
   
   Line()
   {
      start = NULL;
      end = NULL;
      wordNum = 0;
      wordWidthSum = 0;
   }
};

void printLine(float x, float y, float w, char const* lineStart, char const* lineEnd, int wordNum, float wwsum)
{
   char const* pos = lineStart;
   float spacing = wordNum > 1 ? (w - wwsum) / (wordNum - 1) : 0.f;
   
   if(spacing > minSpacing * 1.4f)
     spacing = minSpacing * 1.4f;
   
   x += (w - (wwsum + spacing * (wordNum - 1))) * 0.5f;
   
   while(pos != lineEnd)
   {
       // Skip tabs and spaces:
       while(pos != lineEnd && (*pos == 9 || *pos == 32))
       {
          ++pos;
       }
       
       // Print the word:
       while(pos != lineEnd && *pos != 9 && *pos != 32)
       {
          int ch = ugetxc(&pos);
          x += font->vtable->render_char(font, ch, 0, -1, buffer, int(x), int(y));
       }
       
       x += spacing;
   }
}

char const* findLineEnd(int* wordNum, float* wordWidthSum, char const* lineStart, float w)
{
     char const* pos = lineStart;
     char const* lastFitting = NULL;
     char const* lastEnd = NULL;
     float rem = w;
     
     *wordNum = 0;
     *wordWidthSum = 0.f;
     
     while(*pos)
     {
        lastEnd = pos;
      
        // Skip tabs and spaces:
        while(*pos == 9 || *pos == 32)
        {
          ++pos;
        }
        
        // Iterate over the word:
        float wordW = 0.f;
        float fittingW = 0.f;
        
        while(*pos && *pos != 9 && *pos != 32)
        {
           int ch = ugetxc(&pos);
           float chw = font->vtable->char_length(font, ch);
           
           rem -= chw;
           wordW += chw;
           
           if(rem >= 0.f)
           {
              lastFitting = pos;
              fittingW += chw;
           }
        }
        
        // The last word doesn't fit?
        if(rem < 0.f)
        {
             if(wordW > w) // Wouldn't fit in the next line either? Split it.
             {
                *wordWidthSum += fittingW;
                ++(*wordNum);
                return lastFitting;
             }
             else
                return lastEnd;
        }
        
        else
        {
           rem -= minSpacing;
           *wordWidthSum += wordW;
           ++(*wordNum);
        }
     }
     
     // Last word of the line is the last word of the text and it fits.
     return pos;
}

void print(float x, float y, float w, float h, char const* text)
{
     List<Line> listList;
     char const* pos = text;
     float y0 = y;
 
     rect(buffer, int(x - 2), int(y - 2), int(x + w + 2), int(y + h + 2), makecol(100, 100, 100));

     while(*pos && (y - y0) < h - text_height(font))
     {
         float wwsum;
         int wordNum;
      
         char const* end = findLineEnd(&wordNum, &wwsum, pos, w);
         printLine(x, y, w, pos, end, wordNum, wwsum);
         pos = end;
         y += text_height(font) + 5;
     }
}

int main()
{   
    bool exit = false;
    
    init();
 
    char const* text = "AUXILIARIES, which are the other useless arm, are employed when a prince is called in with his forces to aid and defend, as was done by Pope Julius in the most recent times; for he, having, in the enterprise against Ferrara, had poor proof of his mercenaries, turned to auxiliaries, and stipulated with Ferdinand, King of Spain, for his assistance with men and arms. These arms may be useful and good in themselves, but for him who calls them in they are always disadvantageous; for losing, one is undone, and winning, one is their captive.And although ancient histories may be full of examples, I do not wish to leave this recent one of Pope Julius II, the peril of which cannot fall to be perceived; for he, wishing to get Ferrara, threw himself entirely into the hands of the foreigner. But his good fortune brought about a third event, so that he did not reap the fruit of his rash choice; because, having auxiliaries routed at Ravenna, and the Switzers having risen and driven out the conquerors (against all expectation, both his and others), it so came to pass that he did not become prisoner to his enemies, they having fled, nor to his auxiliaries, he having conquered by other arms than theirs";
    float w = 24.f, h = 98.f;
 

    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
  
     if(key[KEY_UP])
       h -= 0.5f;
     if(key[KEY_DOWN])
       h += 0.5f;
     if(key[KEY_LEFT])
       w -= 0.5f;
     if(key[KEY_RIGHT])
       w += 0.5f;
       
     if(key[KEY_SPACE])
       printf("%f %f\n", w, h);
  
     clear_to_color(buffer, makecol(128, 128, 128));
     
     print(10.f, 10.f, w, h, text);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
