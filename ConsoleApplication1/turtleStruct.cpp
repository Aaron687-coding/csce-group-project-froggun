#include "turtleStruct.h"
#include "bulletStruct.h"
#include <vector>
#include <iostream>
#include "frogClass.h"
#include <cstdlib>
#include <ctime>

const float TURTLE_SPEED = 0.5;
const int TURTLE_HIDE_DISTANCE = 100;
const float BULLET_SPEED = 2;
int Turtle::turtCounter = 0;

using namespace std;

Turtle::Turtle(SDL_Rect r, bool hiding, float dx, float dy, SDL_Texture* tex)
    : rect(r), hiding(hiding), dx(dx), dy(dy), texture(tex), up(0), down(0), left(0), right(0), bulletTimer(0), facingRight(false), turtmoveTimer(500), moveDuration(0) {}

void Turtle::updateMovement() 
{
    {
       
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

                
                cout << "dx: " << dx << ", dy: " << dy << endl;
                if (dx == 0||dx==1)
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
    
}

void Turtle::fireBullet(vector<Bullet>& bullets, Frog& player, float deltaTime, SDL_Renderer* renderer, SDL_Texture* bulletTexture)
{

    if (!hiding)
    {
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

               bullets.push_back(Bullet(bulletStartX, bulletStartY, BULLET_SPEED, bulletTexture, directionX, directionY));
               cout << "Magnitude: " << magnitude << " DirectionX: " << directionX << " DirectionY: " << directionY << endl;

            }


            // Reset the bullet timer
            bulletTimer = 0;  // No fully auto turts
            
        }

        else
        {
            bulletTimer++;
        }

        

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
    SDL_Rect frogRect = player.getCollisionBox();
    int deltaX = frogRect.x + frogRect.w / 2 - (rect.x + rect.w / 2);
    int deltaY = frogRect.y + frogRect.h / 2 - (rect.y + rect.h / 2);

    // Calculate the distance between the turtle and the player
    int distanceaX = deltaX - rect.x;
    int distanceY = deltaY - rect.y;
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

    // Toggle hiding based on distance
    if (distance <= TURTLE_HIDE_DISTANCE)
    {
        hiding = true;
    }
    else
    {
        hiding = false;
    }
}

void Turtle::spawnTurtles(vector<Turtle> &turtles, int frameCount, SDL_Texture* turtleTexture)
{
    if (frameCount % 500 == 0) 
    {
        if (turtCounter < 2)
        {
            Turtle turtle = { { rand() % (1280 - 50), rand() % (720 - 50), 50, 50 }, false, 0, 0, turtleTexture };
            turtles.push_back(turtle);
            turtCounter++;
        }
        
    }
}