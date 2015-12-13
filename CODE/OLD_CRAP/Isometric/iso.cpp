#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

void init()
{
     allegro_init();
     set_color_depth(32);
}

int main()
{
    bool exit = false;
    
    float w = 101, h = 60, k = 1.0;
    printf("W, H, K:\n");
    scanf("%f %f %f", &w, &h, &k);
    
    int wi = int(w), hi = int(h), zi = int(sqrt(w * w / 4 + h * h / 4) * k);
    
    init();
    BITMAP* tile = create_bitmap(wi, hi + zi);
    int points1[] = {0, hi/2,
                     wi/2, 0,
                     wi - 1, hi/2,
                     wi - 1, hi/2 + zi,
                     wi/2,   hi + zi,
                     0, hi/2 + zi};
    
    int points2[] = {0, zi + hi/2,
                     wi/2, zi,
                     wi - 1, zi + hi / 2,
                     wi/2, hi + zi};
    
    clear_to_color(tile, makecol(0, 0, 0));
    polygon(tile, 6, points1, makecol(255, 0, 255));
    polygon(tile, 4, points2, makecol(255, 255, 255));
    
    char str[200];
    
    sprintf(str, "tile%dx%dx%d.bmp", wi, hi, zi);
    
    save_bitmap(str, tile, NULL);
    
    destroy_bitmap(tile);
    return 0;
}END_OF_MAIN()
