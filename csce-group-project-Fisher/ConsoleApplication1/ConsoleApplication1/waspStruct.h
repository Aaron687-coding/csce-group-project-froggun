#ifndef WASP_H
#define WASP_H

#include <SDL.h>

struct Wasp {
    SDL_Rect rect;
    SDL_Texture* texture;
    int dx, dy;
    int left, right;
    bool active;
    bool facingRight;

    Wasp(SDL_Rect r, int dx, int dy, SDL_Texture* tex);
    void move();
    void moveTowards(const Wasp& player, int speed);
};

#endif
