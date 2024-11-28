#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>
#include <string>

struct Bullet 
{
    SDL_Rect rect;
    SDL_Texture* texture;
    float speed;
    int dx, dy;

    Bullet(int x, int y, int s, SDL_Texture* tex, int dirX, int dirY);
    void move();
};

#endif
