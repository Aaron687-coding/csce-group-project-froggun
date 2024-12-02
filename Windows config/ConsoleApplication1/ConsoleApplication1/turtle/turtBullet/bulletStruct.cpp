#include "bulletStruct.h"
#include <iostream>
#include "../../frog/frogClass.h"

Bullet::Bullet(int x, int y, int s, SDL_Texture* tex, float dirX, float dirY)
    : rect{ x, y, 40, 40 }, speed(s), directionX(dirX), directionY(dirY), texture(tex), active(true) {}

void Bullet::move(float deltaTime, Frog& player) 
{
    SDL_Rect frogRect = player.getCollisionBox();
    rect.x += directionX * speed;
    rect.y += directionY * speed;

    if (SDL_HasIntersection(&rect, &frogRect)) 
    {
        active = false;
    }
}
