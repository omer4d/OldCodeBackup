#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include <inttypes.h>
#include <float.h>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "Vec3f.hpp"
#include "DrawVec2f.hpp"
#include "Perlin.hpp"
#include "Buffer.hpp"

#include "GaussBlur.hpp"
#include "ClipRect.hpp"

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

float randomDither(float val, int levels)
{
    float step = 1.f / float(levels);
    float nearest = int(val / step) * step;
    float error = (val - nearest) / step;
    
    return randf() > error ? nearest : (nearest + step);
}

int thresh(int in, int level)
{
   return in < level ? 0 : 255;
}

int lum(int col)
{
   return int(0.2126 * getr(col) + 0.7152 * getg(col) + 0.0722 * getb(col));
}

int clamp0255(int in)
{
   if(in < 0) return 0;
   else if(in > 255) return 255;
   else return in;
}

void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;
	min = MIN(MIN( r, g), b );
	max = MAX(MAX( r, g), b );
	*v = max;				// v
	delta = max - min;
	if( max != 0 )
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}
	if( r == max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan
	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;
}
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = int( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* image = load_bitmap("lena.bmp", NULL);
    
    float q = 1;
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;

     clear_to_color(buffer, 0);

     for(int j = 0; j < image->h; ++j)
        for(int i = 0; i < image->w; ++i)
        {           
           int c = PIXEL(image, i, j);
           float r0 = getr32(c) / 255.f, g0 = getg32(c) / 255.f, b0 = getb32(c) / 255.f;
           float l = lum(c) / 255.f;
           
           float h, s, v;
           float r1, g1, b1;
           
           RGBtoHSV(r0, g0, b0, &h, &s, &v);
           
           v = randomDither(v, 5);
           //v = v * 0.3 + randomDither(v, 1) * 0.7;
           //v *= 0.5;
           //if(v > 1)
           //v = 1;
           
           HSVtoRGB(&r1, &g1, &b1, h, s, v);
           
           /*
           float r1 = (r0 / l) * 0.4;
           float g1 = (g0 / l) * 0.4;
           float b1 = (b0 / l) * 0.4;
           */
           
           //if(r1 > 1) r1 = 1;
           //if(g1 > 1) g1 = 1;
           //if(b1 > 1) b1 = 1;
           
           
           //float r1 = randomDither(r0, 1) * q + r0 * (1 - q);
           //float g1 = randomDither(g0, 1) * q + g0 * (1 - q);
           //float b1 = randomDither(b0, 1) * q + b0 * (1 - q);
           
           PIXEL(buffer, i, j) = makecol32(int(r1 * 255), int(g1 * 255), int(b1 * 255));
        }
        
        //getch();

     //draw_sprite(buffer, image, 0, 0);
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(image);
    deinit();
    return 0;
}END_OF_MAIN()
