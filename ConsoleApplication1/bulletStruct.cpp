#include "bulletStruct.h"
#include <iostream>
#include "frogClass.h"

Bullet::Bullet(int x, int y, int s, SDL_Texture* tex, float dirX, float dirY)
    : rect{ x, y, 40, 40 }, speed(s), directionX(dirX), directionY(dirY), texture(tex) {}

void Bullet::move(float deltaTime, Frog& player) 
{
    SDL_Rect frogRect = player.getCollisionBox();
   // cout << deltaTime << "Bullet Speed: " << speed << " | Position: (" << rect.x << ", " << rect.y << ")\n";
    rect.x += directionX * speed;
    rect.y += directionY * speed;

    if (SDL_HasIntersection(&rect, &frogRect)) 
    {
        active = false;
    }
    
}
