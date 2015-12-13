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
    bool enabled;
    int x, y;
    
public:

    InputField(int charNum, int x=0, int y=0) : text("")
    {
        this->enabled = true;
        this->charNum = charNum;
        text.reserve(charNum);
        iter = text.end();
        caret = 0;
        this->x = x;
        this->y = y;
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
        if(!enabled)
            return;
        
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

    bool isInside(int px, int py)
    {
        int w = 8, h = 10, n = charNum;
        return px > x && px < (x + w * n) && py > y && py < (y + h);
    }
    
    void render(BITMAP* buffer)
    {
        int w = 8, h = 10, n = charNum;
        int col = enabled ? makecol(240, 240, 255) : makecol(24, 24, 25);
        
        rectfill(buffer, x, y, x + w * n, y + h, col);
        rect(buffer, x, y, x + w * n, y + h, makecol(0, 0, 100));
        
        textprintf_ex(buffer, font, x + 2, y + 2, 0, -1, "%s", text.c_str());
        line(buffer, x + caret * w, y, x + caret * w, y + h, makecol(255, 0, 0));
    }

    void setEnabled(bool enabled)
    {
        this->enabled = enabled;
    }

    bool isEnabled() const
    {
        return enabled;
    }
};

#endif	/* INPUTFIELD_HPP */

