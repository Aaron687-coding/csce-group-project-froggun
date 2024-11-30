#include "waspStruct.h"
#include <cmath>
#include <vector>
#include "../frog/frogClass.h"
#include <cstdlib>

using namespace std;

Wasp::Wasp(SDL_Rect r, float dx, float dy, SDL_Texture* tex)
    : rect(r), texture(tex), dx(dx), dy(dy), left(0), right(0), active(true), 
      facingRight(false), health(nullptr), pendingRemoval(false) {}

void Wasp::moveTowards(Frog& player, int speed)
{
    if (pendingRemoval) return;  // Don't move if pending removal

    SDL_Rect frogRect = player.getCollisionBox();
    float deltaX = frogRect.x + frogRect.w / 2 - (rect.x + rect.w / 2);
    float deltaY = frogRect.y + frogRect.h / 2 - (rect.y + rect.h / 2);

    float magnitude = sqrt(deltaX * deltaX + deltaY * deltaY);
    if (magnitude != 0)
    {
        // Calculate direction and apply speed
        dx = (static_cast<float>(speed) * deltaX) / magnitude;
        dy = (static_cast<float>(speed) * deltaY) / magnitude;

        rect.x = static_cast<int>(rect.x + dx);
        rect.y = static_cast<int>(rect.y + dy);
    }

    if (dx > 0)
    {
        facingRight = true;
    }
    if (dx < 0)
    {
        facingRight = false;
    }
}

void Wasp::spawnWasps(vector<Wasp>& wasps, int frameCount, SDL_Texture* waspTexture, SDL_Renderer* renderer)
{
    if (frameCount % 300 == 0)  // Spawn every 300 frames
    {
        // Create a new wasp at a random position along the edges
        int x, y;
        int side = rand() % 4;  // 0: top, 1: right, 2: bottom, 3: left

        switch (side) {
            case 0:  // top
                x = rand() % 1280;
                y = 0;
                break;
            case 1:  // right
                x = 1280;
                y = rand() % 720;
                break;
            case 2:  // bottom
                x = rand() % 1280;
                y = 720 - (16 * 3); // Spawn them in view
                break;
            case 3:  // left
                x = 16 * 3; // Spawn them in view
                y = rand() % 720;
                break;
            default:
                x = 0;
                y = 0;
        }

        SDL_Rect waspRect = { x, y, 16 * 3, 16 * 3 };  // scale up image size by three
        
        // Reserve space in the vector before adding new element
        wasps.reserve(wasps.size() + 1);
        
        // Create wasp directly in the vector
        wasps.emplace_back(waspRect, 0.0f, 0.0f, waspTexture);
        
        // Initialize health bar for the newly added wasp
        if (renderer) {
            wasps.back().initHealthBar(renderer);
        }
    }
}
