#include "bulletStruct.h"

Bullet::Bullet(int x, int y, int s, SDL_Texture* tex, int dirX, int dirY)
    : rect{ x, y, 40, 40 }, speed(s), dx(dirX), dy(dirY), texture(tex) {}

void Bullet::move() 
{
    rect.x += dx * speed;
    rect.y += dy * speed;
}
