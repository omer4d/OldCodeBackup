/* 
 * File:   Mouse.hpp
 * Author: Stas
 *
 * Created on June 15, 2012, 4:49 PM
 */

#ifndef MOUSE_HPP
#define	MOUSE_HPP

#include <allegro.h>

class Mouse {
    int lastMb, sx, sy;

public:
    Mouse()
    {
        lastMb = 0;
        sx = 0;
        sy = 0;
    }
    
    void update()
    {
        if(mouse_b && !lastMb)
        {
            sx = mouse_x;
            sy = mouse_y;
        }
        
        lastMb = mouse_b;
    }
    
    bool buttonPressed(int i)
    {
        return mouse_b == i && lastMb != i;
    }
    
    bool buttonReleased(int i)
    {
        return mouse_b != i && lastMb == i;
    }
    
    bool buttonHeld(int i)
    {
        return mouse_b == i;
    }
    
    int getSourceX()
    {
        return sx;
    }
    
    int getSourceY()
    {
        return sy;
    }
    
    int getDragX()
    {
        return mouse_x - sx;
    }
    
    int getDragY()
    {
        return mouse_y - sy;
    }
};

#endif	/* MOUSE_HPP */

