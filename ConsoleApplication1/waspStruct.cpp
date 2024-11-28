#include "waspStruct.h"
#include <cmath>
#include <vector>
#include "frogClass.h"


using namespace std;

Wasp::Wasp(SDL_Rect r, int dx, int dy, SDL_Texture* tex)
    : rect(r), dx(dx), dy(dy), texture(tex), active(true), facingRight(false), left(0), right(0) {}

void Wasp::move()
{
    if (active)
    {
        rect.x += static_cast<int>(dx);
        rect.y += static_cast<int>(dy);

    }
}

void Wasp::moveTowards(Frog& player, int speed)
{
    SDL_Rect frogRect = player.getCollisionBox();
    int deltaX = frogRect.x + frogRect.w / 2 - (rect.x + rect.w / 2);
    int deltaY = frogRect.y + frogRect.h / 2 - (rect.y + rect.h / 2);

    float magnitude = sqrt(deltaX * deltaX + deltaY * deltaY);
    if (magnitude != 0)
    {
        int directionX = speed * deltaX / magnitude;
        int directionY = speed * deltaY / magnitude;

        // Update dx and dy directly with float values
        dx = (directionX *= 1 / sqrt(2));
        dy = (directionY *= 1 / sqrt(2));


    }

    if (dx > 0)
    {
        facingRight = true;
    }
    if (dx < 0)
    {
        facingRight = false;
    }

    if (SDL_HasIntersection(&rect, &frogRect)) {
        active = false;  
    }
}

void Wasp::spawnWasps(vector<Wasp>& wasps, int frameCount, SDL_Texture* waspTexture)
{
    if (frameCount % 100 == 0)
    {
        Wasp wasp = { { rand() % (1280 - 50), 0, 50, 50 }, 0, 0, waspTexture };
        wasps.push_back(wasp);
    }
}