#ifndef WASP_H
#define WASP_H

#include <SDL.h>
#include <vector>
#include "frogClass.h"
#include <SDL.h>
#include <SDL_image.h>
using namespace std;

struct Wasp 
{
    SDL_Rect rect;
    SDL_Texture* texture;
    int dx, dy;
    int left, right;
    bool active;
    bool facingRight;

    Wasp(SDL_Rect r, int dx, int dy, SDL_Texture* tex);

    void move();
    void moveTowards(Frog& player, int speed);
    static void spawnWasps(vector<Wasp>& wasps, int frameCount, SDL_Texture* waspTexture);
};

#endif
