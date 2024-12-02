#include "turtleStruct.h"
#include "turtBullet/bulletStruct.h"
#include <vector>
#include <algorithm>  // for remove_if
#include <ctime>
#include <cstdlib>
#include <iostream>

const float TURTLE_SPEED = 0.5f;
const int TURTLE_MOVE_INTERVAL = 120;
const int TURTLE_HIDE_DISTANCE = 100;
const float BULLET_SPEED = 2.0f;
int Turtle::turtCounter = 0;

using namespace std;

Turtle::Turtle(SDL_Rect r, bool hiding, float dx, float dy, SDL_Texture* tex)
    : rect(r), texture(tex), hiding(hiding), dx(dx), dy(dy), up(0), down(0), left(0), right(0), 
      bulletTimer(0), facingRight(false), turtmoveTimer(500), moveDuration(0), health(nullptr),
      pendingRemoval(false) {}

void Turtle::updateMovement() 
{
    if (pendingRemoval) return;  // Don't move if pending removal

    if (!hiding)
    {
        if (turtmoveTimer == 0)
        {
            // rand movement at rand times
            dx = (rand() % 3 - 1);
            dy = (rand() % 3 - 1);

            while (dx == 0 && dy == 0)
            {
                dx = (rand() % 3 - 1); //no more lazy turtles
                dy = (rand() % 3 - 1);
            }

            std::cout << "dx: " << dx << ", dy: " << dy << std::endl;
            if (dx == 0 || dx == 1)
            {
                dx = 1;
                facingRight = true;
            }

            if (dx == -1)
            {
                facingRight = false;
            }

            turtmoveTimer = 500;
            moveDuration = 300;
        }
        if (moveDuration > 0)
        {
            rect.x += dx;
            rect.y += dy;

            moveDuration--;  
        }
        else
        {
            dx = 0;
            dy = 0;
        }

        turtmoveTimer--;

        if (rect.x <= 0 || rect.x + rect.w >= 1280)
        {
            dx = -dx;
        }

        if (rect.y <= 0 || rect.y + rect.h >= 720)
        {
            dy = -dy;
        }

        // no escape
        if (rect.x < 0) rect.x = 0;
        if (rect.y < 0) rect.y = 0;
        if (rect.x + rect.w > 1280) rect.x = 1280 - rect.w;
        if (rect.y + rect.h > 720) rect.y = 720 - rect.h;
    }
}

void Turtle::fireBullet(vector<Bullet>& bullets, Frog& player, float deltaTime, SDL_Renderer* renderer, SDL_Texture* bulletTexture)
{
    if (hiding || pendingRemoval) return;  // Don't fire if hiding or pending removal

    // dang turtles with guns
    if (bulletTimer >= 300)
    {
        SDL_Rect frogRect = player.getCollisionBox();
        int deltaX = frogRect.x + frogRect.w / 2 - (rect.x + rect.w / 2);
        int deltaY = frogRect.y + frogRect.h / 2 - (rect.y + rect.h / 2);

        float magnitude = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (magnitude != 0)
        {
            float directionX = BULLET_SPEED * (deltaX / magnitude);
            float directionY = BULLET_SPEED * (deltaY / magnitude);

            int bulletStartX = rect.x + rect.w / 2;
            int bulletStartY = rect.y + rect.h / 2;

            bullets.reserve(bullets.size() + 1);
            bullets.emplace_back(bulletStartX, bulletStartY, BULLET_SPEED, bulletTexture, directionX, directionY);
            std::cout << "Magnitude: " << magnitude << " DirectionX: " << directionX << " DirectionY: " << directionY << std::endl;
        }

        // Reset the bullet timer
        bulletTimer = 0;  // No fully auto turts
    }
    else
    {
        bulletTimer++;
    }

    for (auto& bullet : bullets)
    {
        bullet.move(deltaTime, player);
    }

    bullets.erase(
        remove_if(bullets.begin(), bullets.end(), [](Bullet& b)
            {
                return b.rect.y < -b.rect.h || b.rect.y > 720 || b.rect.x < 0 || b.rect.x > 1280;
            }),
        bullets.end());
}

void Turtle::hideinShell(Frog& player)
{
    if (pendingRemoval) return;  // Don't change hiding state if pending removal

    SDL_Rect frogRect = player.getCollisionBox();
    int deltaX = frogRect.x + frogRect.w / 2 - (rect.x + rect.w / 2);
    int deltaY = frogRect.y + frogRect.h / 2 - (rect.y + rect.h / 2);

    // Calculate the distance between the turtle and the player
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

    // Toggle hiding based on distance
    if (distance <= TURTLE_HIDE_DISTANCE)
    {
        hiding = true;
    }
    else if (!pendingRemoval)  // Only come out of hiding if not pending removal
    {
        hiding = false;
    }
}

void Turtle::spawnTurtles(vector<Turtle> &turtles, int frameCount, SDL_Texture* turtleTexture, SDL_Renderer* renderer, int maxTurts)
{
    if (frameCount % 500 == 0) 
    {
        if (turtCounter < maxTurts || maxTurts == 0) // Override limit with maxTurts = 0
        {
            SDL_Rect newRect = { rand() % (1280 - 50), rand() % (720 - 50), 32 * 3, 19 * 3 };
            
            // Reserve space in the vector before adding new element
            turtles.reserve(turtles.size() + 1);
            
            // Create turtle directly in the vector
            turtles.emplace_back(newRect, false, 0, 0, turtleTexture);
            
            // Initialize health bar for the newly added turtle
            if (renderer) {
                turtles.back().initHealthBar(renderer);
            }
            
            turtCounter++;
        }
    }
}
