#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>
#include <string>
#include <frogClass.h>

struct Bullet 
{
    SDL_Rect rect;
    SDL_Texture* texture;
    float speed = 1;
    float directionX;
    float directionY;
    bool active;

    Bullet(int x, int y, int s, SDL_Texture* tex, float dirX, float dirY);
    void move(float deltaTime, Frog& player);
};

#endif
