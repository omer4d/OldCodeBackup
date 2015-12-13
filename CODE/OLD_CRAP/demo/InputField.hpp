/* 
 * File:   InputField.hpp
 * Author: Stas
 *
 * Created on February 2, 2012, 11:27 PM
 */

#ifndef INPUTFIELD_HPP
#define	INPUTFIELD_HPP

#include <string>
#include <allegro.h>

class InputField {
    std::string text;
    std::string::iterator iter;
    int caret, charNum;
    
public:

    InputField(int charNum) : text("")
    {
        this->charNum = charNum;
        text.reserve(charNum);
        iter = text.end();
        caret = 0;
    }
    
    void clear()
    {
        text.clear();
        iter = text.end();
        caret = 0;
    }
    
    std::string getText()
    {
        return text;
    }

    void keyPressed(int ascii, int scan)
    {
        if(ascii >= 32 && ascii <= 126 && text.length() < charNum)
        {
            iter = text.insert(iter, ascii);
            caret++;
            ++iter;
        }
        
        switch(scan)
        {
            case KEY_LEFT:
                if(caret > 0)
                {
                    caret--;
                    --iter;
                }
                break;
            case KEY_RIGHT:
                if(caret < text.length())
                {
                    caret++;
                    ++iter;
                }
                break;
            case KEY_BACKSPACE:
                if(caret > 0)
                {
                    caret--;
                    --iter;
                    iter = text.erase(iter);
                }
                break;
        }
    }

    void render(BITMAP* buffer, int x, int y)
    {
        int w = 8, h = 10, n = charNum;
        rectfill(buffer, x, y, x + w * n, y + h, makecol(240, 240, 255));
        rect(buffer, x, y, x + w * n, y + h, makecol(0, 0, 100));
        
        textprintf_ex(buffer, font, x + 2, y + 2, 0, -1, "%s", text.c_str());
        line(buffer, x + caret * w, y, x + caret * w, y + h, makecol(255, 0, 0));
    }
};

#endif	/* INPUTFIELD_HPP */

