#ifndef TURTLE_H
#define TURTLE_H

#include <SDL2/SDL.h>
#include <vector>
#include "turtBullet/bulletStruct.h"

struct Turtle 
{
    const int TURTLE_SPEED = 1;
    const int TURTLE_HIDE_DISTANCE = 100;
    const int TURTLE_MOVE_INTERVAL = 120;

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
    void fireBullet(std::vector<Bullet>& bullets, SDL_Texture* bulletTexture);
    void hideinShell(SDL_Renderer* renderer, const Turtle& player);
    void spawnTurtles(std::vector<Turtle>& turtles, SDL_Renderer* renderer, int frameCount, SDL_Texture* turtleTexture);
};

#endif
