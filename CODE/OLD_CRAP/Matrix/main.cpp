#include <allegro.h>
#include <stdio.h>
#include <math.h>
#include <stdexcept>

#include <vector>

BITMAP *buffer;

void init()
{
    allegro_init();
    set_color_depth(32);

    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0))
    {
        allegro_message("%s.", allegro_error);
        exit(1);
    }

    if(install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL))
        allegro_message("Error: %s.", allegro_error);

    install_keyboard();
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

void noteOn(int chan, int note, int vel)
{
    unsigned char msg[3] = {0x90 + chan, note, vel};
    midi_out(msg, sizeof(msg));
}

void noteOff(int chan, int note)
{
    unsigned char msg[3] = {0x80 + chan, note, 0};
    midi_out(msg, sizeof(msg));
}

int main()
{
    int exit = 0;

    init();
    
    noteOn(0, 60, 127);
    noteOn(0, 62, 127);
    noteOn(0, 65, 127);
    noteOn(0, 68, 127);
    
    while(!exit)
    {
        if(key[KEY_ESC])
        {
            exit = 1;
        }

        clear_to_color(buffer, makecol32(64, 64, 64));
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

    destroy_bitmap(buffer);
    return 0;
}
END_OF_MAIN()
