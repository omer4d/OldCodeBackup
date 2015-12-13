#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <allegro.h>
#include <list>
#include <iostream>
#include <sstream>

#include "CommandParser.hpp"
#include "Lexcast.hpp"

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
	srand(time(NULL));
}

void deinit()
{
	destroy_bitmap(buffer);
}

struct Settings {
	std::vector<std::string> inVec;
	std::string out, patch1, patch2;
	bool calcDiff;
	int rmin, rmax;
};

Settings parseSettings()
{
	Settings settings;
	CommandParser parser;
	
	parser.onError([&](CommandParser::Error error, std::string const& cmdName)
	{
		printf("Unrecognized command: %s\n", cmdName.c_str());
	});
	
	parser.reg<int, int, float, std::string>("-cmd1", [&](int a, int b, float c, std::string const& str) 
	{
		printf("%d %d %f %s", a, b, c, str.c_str());
	}, {"5.7", "lala"});
	
	parser.parse("-cmd1 1 3 6.6");
	
	return settings;
}

int main()
{
	bool exit = false;
	
	init();
	
	parseSettings();

	while(!exit)
	{
		if(key[KEY_ESC]) exit = true;
		
		clear_to_color(buffer, 0);
		blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	}
	
	deinit();
	return 0;
}
END_OF_MAIN()
