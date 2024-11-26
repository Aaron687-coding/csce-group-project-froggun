#include "waspStruct.h"
#include <cmath>

Wasp::Wasp(SDL_Rect r, int dx, int dy, SDL_Texture* tex)
    : rect(r), dx(dx), dy(dy), texture(tex), active(true), facingRight(false), left(0), right(0) {}

void Wasp::move() {
    rect.x += dx;
    rect.y += dy;
}

void Wasp::moveTowards(const Wasp& player, int speed) {
    int deltaX = player.rect.x + player.rect.w / 2 - (rect.x + rect.w / 2);
    int deltaY = player.rect.y + player.rect.h / 2 - (rect.y + rect.h / 2);

    float magnitude = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    if (magnitude != 0) {
        dx = static_cast<int>(speed * deltaX / magnitude);
        dy = static_cast<int>(speed * deltaY / magnitude);
    }

    facingRight = dx > 0;
}
