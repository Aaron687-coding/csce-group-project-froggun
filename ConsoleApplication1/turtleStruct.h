#ifndef TURTLE_H
#define TURTLE_H

#include <SDL.h>
#include <vector>
#include "bulletStruct.h"
#include "frogClass.h"


using namespace std;

struct Turtle
{
    SDL_Rect rect;
    SDL_Texture* texture;
    int up, down, left, right;
    bool hiding;
    float dx, dy;
    int bulletTimer;
    bool facingRight;
    int turtmoveTimer;     
    int moveDuration;
    static int turtCounter;

    Turtle(SDL_Rect r, bool hiding, float dx, float dy, SDL_Texture* tex = nullptr);

    void updateMovement();
    void fireBullet(vector<Bullet>& bullets, Frog& player, float deltaTime, SDL_Renderer* renderer, SDL_Texture* bulletTexture);
    void hideinShell(Frog& player);
    static void spawnTurtles(vector<Turtle>& turtles, int frameCount, SDL_Texture* turtleTexture);
};

#endif