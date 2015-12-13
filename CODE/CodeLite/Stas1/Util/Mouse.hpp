/* 
 * File:   Mouse.hpp
 * Author: Stas
 *
 * Created on July 5, 2012, 9:24 AM
 */

#ifndef MOUSE_HPP
#define	MOUSE_HPP

#include <allegro.h>
#include "Vec2f.hpp"

class Mouse {
    Vec2f pos, delta;
    static Mouse* instance;
    int lastMb;

public:

    Mouse()
    {
        pos.init(mouse_x, mouse_y);
    }

    static Mouse& getInstance()
    {
        if(!Mouse::instance)
            Mouse::instance = new Mouse();

        return *Mouse::instance;
    }

    void update()
    {
        int br = int(MIN(SCREEN_W, SCREEN_H) * 0.3);

        if(mouse_x < br || mouse_x > SCREEN_W - br || mouse_y < br || mouse_y > SCREEN_H - br)
            position_mouse(SCREEN_W / 2, SCREEN_H / 2);

        int dx, dy;
        get_mouse_mickeys(&dx, &dy);
        delta.init(dx, dy);

        pos += delta;

        if(pos.x < 0) pos.x = 0;
        if(pos.x > SCREEN_W - 1) pos.x = SCREEN_W - 1;
        if(pos.y < 0) pos.y = 0;
        if(pos.y > SCREEN_H - 1) pos.y = SCREEN_H - 1;

        lastMb = mouse_b;
    }

    Vec2f const& getPos() const
    {
        return pos;
    }

    Vec2f const& getDelta() const
    {
        return delta;
    }

    int held() const
    {
        return mouse_b;
    }

    int pressed() const
    {
        return(mouse_b && !lastMb) ? mouse_b : 0;
    }

    int released() const
    {
        return(!mouse_b && lastMb) ? lastMb : 0;
    }

    bool held(int b) const
    {
        b = 1 << (b - 1);
        return mouse_b & b;
    }

    bool pressed(int b) const
    {
        b = 1 << (b - 1);
        return(mouse_b & b) && !(lastMb & b);
    }

    bool released(int b) const
    {
        b = 1 << (b - 1);
        return !(mouse_b & b) && (lastMb & b);
    }
};

Mouse* Mouse::instance = NULL;

#endif	/* MOUSE_HPP */

