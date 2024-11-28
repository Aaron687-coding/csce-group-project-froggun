#ifndef TURTLE_H
#define TURTLE_H

#include <SDL.h>
#include <vector>
#include "bulletStruct.h"

struct Turtle 
{
    SDL_Rect rect;
    SDL_Texture* texture;
    int up, down, left, right;
    bool hiding;
    int dx, dy;
    int moveTimer;
    int bulletTimer;
    bool facingRight;

    Turtle(SDL_Rect r, bool hiding, int dx, int dy, int moveTimer, SDL_Texture* tex = nullptr);
    void updateMovement();
    void fireBullet(std::vector<Bullet>& bullets, SDL_Renderer* renderer, SDL_Texture* bulletTexture);
    void hideinShell(SDL_Renderer* renderer, const Turtle& player);
    void spawnTurtles(std::vector<Turtle>& turtles, SDL_Renderer* renderer, int frameCount, SDL_Texture* turtleTexture);
};

#endif
