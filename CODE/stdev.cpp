#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>

#include "Util.hpp"
#include "Buffer.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

void deinit()
{
     destroy_bitmap(buffer);
}

float squared(float n)
{
	return n * n;
}

float normRandf(float mean, float dev)
{
	return mean + ((Util::randf() * 2 - 1) + 
				   (Util::randf() * 2 - 1) + 
				   (Util::randf() * 2 - 1)) * dev;
}

void stdev1(std::vector<float> const& samples)
{
	float sum = 0.f;
	
    for(int i = 0; i < samples.size(); ++i)
    	sum += samples.at(i);
    
    float avg = sum / samples.size();
    
	printf("Mean: %f\n", avg);
	
	float sqDiffSum = 0.f;
  
    for(int i = 0; i < samples.size(); ++i)
    	sqDiffSum += squared(samples.at(i) - avg);
    
    printf("Stdev: %f\n", sqrt(sqDiffSum / samples.size()));
}

void stdev2(std::vector<float> const& samples)
{
	float sum = 0.f, sqSum = 0.f;
	
	for(int i = 0; i < samples.size(); ++i)
	{
		sum += samples.at(i);
		sqSum += squared(samples.at(i));
	}
	
	float mean = sum / samples.size();
	
	printf("Mean: %f\n", mean);
	printf("Stdev: %f\n", sqrt((sqSum - 2 * mean * sum + samples.size() * mean * mean) / samples.size()));
}

int main()
{
    bool exit = false;
    
    init();
    
    std::vector<float> samples;
    
    for(int i = 0; i < 10000; ++i)
    	samples.push_back(normRandf(0.3, 0.45f));
    	
    stdev1(samples);
    stdev2(samples);
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, makecol(64, 64, 64));

     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
