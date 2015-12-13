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
float minVSpacing = 5.f;

const int TEXT_JUSTIFIED = 0x1;
const int TEXT_H_CENTERED = 0x2;
const int TEXT_V_CENTERED = 0x4;

struct Line {
   char const* start;
   char const* end;
   int wnum;
   float wwsum;
   
   Line()
   {
      start = NULL;
      end = NULL;
      wnum = 0;
      wwsum = 0;
   }
};

Line getLine(char const* lineStart, float w)
{
     char const* pos = lineStart;
     char const* lastFitting = NULL;
     char const* lastEnd = NULL;
     float rem = w;
     Line line;
     
     line.start = lineStart;
     
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
             if(wordW > w && fittingW > 0.f) // Wouldn't fit in the next line either? Split it.
             {
                line.wwsum += fittingW;
                ++line.wnum;
                line.end = lastFitting;
                
                return line;
             }
             
             else
             {
                line.end = lastEnd;
                return line;
             }
        }
        
        else
        {
           rem -= minSpacing;
           line.wwsum += wordW;
           ++line.wnum;
        }
     }
     
     // Last word of the line is the last word of the text and it fits.
     line.end = pos;
     return line;
}

void printLine(float x, float y, float w, Line const& line, int flags)
{
   char const* pos = line.start;
   float spacing;
   
   if(flags & TEXT_JUSTIFIED)
   {
      spacing = line.wnum > 1 ? (w - line.wwsum) / (line.wnum - 1) : 0.f;
   
      if(spacing > minSpacing * 1.4f)
         spacing = minSpacing * 1.4f;
   }
   
   else
      spacing = minSpacing;
   
   if(flags & TEXT_H_CENTERED)
      x += (w - (line.wwsum + spacing * (line.wnum - 1))) * 0.5f;
   
   while(pos != line.end)
   {
       // Skip tabs and spaces:
       while(pos != line.end && (*pos == 9 || *pos == 32))
       {
          ++pos;
       }
       
       // Print the word:
       while(pos != line.end && *pos != 9 && *pos != 32)
       {
          int ch = ugetxc(&pos);
          x += font->vtable->render_char(font, ch, 0, -1, buffer, int(x), int(y));
       }
       
       x += spacing;
   }
}

void print(float x, float y, float w, float h, char const* text, int flags=0)
{
     List<Line> lineList;
     char const* pos = text;
     float y0 = y;
     float lhsum = 0.f;
     bool overflow = false;
     
     rect(buffer, int(x - 2), int(y - 2), int(x + w + 2), int(y + h + 2), makecol(100, 100, 100));

     while(*pos)
     {
         if( (y - y0) > h - text_height(font) )
         {
           overflow = true;
           break;
         }
      
         Line line = getLine(pos, w);
         lineList.pushBack(line);
         y += text_height(font) + minVSpacing;
         lhsum += text_height(font);
         pos = line.end;
     }
     
     y = y0;
     
     if(flags & TEXT_V_CENTERED && !overflow)
        y += (h - (lhsum + 5 * (lineList.getSize() - 1))) * 0.5f;
     
     for(List<Line>::Node* node = lineList.getFirst(); node != NULL; node = node->getNext())
     {
         printLine(x, y, w, node->data, flags);
         y += text_height(font) + minVSpacing;
     }
}

int main()
{   
    bool exit = false;
    
    init();
 
    char const* text = "AUXILIARIES, which are the other useless arm, are employed when a prince is called in with his forces to aid and defend, as was done by Pope Julius in the most recent times; for he, having, in the enterprise against Ferrara, had poor proof of his mercenaries, turned to auxiliaries, and stipulated with Ferdinand, King of Spain, for his assistance with men and arms. These arms may be useful and good in themselves, but for him who calls them in they are always disadvantageous; for losing, one is undone, and winning, one is their captive.And although ancient histories may be full of examples, I do not wish to leave this recent one of Pope Julius II, the peril of which cannot fall to be perceived; for he, wishing to get Ferrara, threw himself entirely into the hands of the foreigner. But his good fortune brought about a third event, so that he did not reap the fruit of his rash choice; because, having auxiliaries routed at Ravenna, and the Switzers having risen and driven out the conquerors (against all expectation, both his and others), it so came to pass that he did not become prisoner to his enemies, they having fled, nor to his auxiliaries, he having conquered by other arms than theirs";
    float w = 38.f, h = 384.f;
 

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
     
     print(10.f, 10.f, w, h, text, TEXT_JUSTIFIED | TEXT_H_CENTERED | TEXT_V_CENTERED);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
