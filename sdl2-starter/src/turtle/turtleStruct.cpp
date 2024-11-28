#include "turtleStruct.h"
#include "turtBullet/bulletStruct.h"
#include <vector>
#include <algorithm>  // for remove_if

const int TURTLE_SPEED = 1;
const int TURTLE_MOVE_INTERVAL = 120;
const int TURTLE_HIDE_DISTANCE = 100;
const int BULLET_SPEED = 1;

Turtle::Turtle(SDL_Rect r, bool hiding, int dx, int dy, int moveTimer, SDL_Texture* tex)
    : rect(r), texture(tex), hiding(hiding), dx(dx), dy(dy), moveTimer(moveTimer), up(0), down(0), left(0), right(0), bulletTimer(0), facingRight(false) {}

void Turtle::updateMovement() 
{
    if (!hiding)
    {
        if (moveTimer == 0)
        {
            // rand movement at rand times
            dx = (rand() % 3 - 1) * TURTLE_SPEED;  // -1, 0, or 1
            dy = (rand() % 3 - 1) * TURTLE_SPEED;  // -1, 0, or 1
            moveTimer = TURTLE_MOVE_INTERVAL;

            if (dx == 1)
            {
                left = 0;
                right = 1;
            }

            if (dx == -1)
            {
                right = 0;
                left = 1;
            }

            if (right)
            {
                facingRight = false;
            }

            if (left)
            {
                facingRight = true;
            }

            if (dy == 1)
            {
                down = 0;
                up = 1;
            }

            if (dy == -1)
            {
                up = 0;
                down = 1;
            }
        }
        else
        {
            moveTimer--;
        }

        // movement
        rect.x += dx;
        rect.y += dy;

        // no escape
        if (rect.x < 0) rect.x = 0;
        if (rect.y < 0) rect.y = 0;
        //if (rect.x + rect.w > SCREEN_WIDTH) rect.x = SCREEN_WIDTH - rect.w;
        //if (rect.y + rect.h > SCREEN_HEIGHT) rect.y = SCREEN_HEIGHT - rect.h;
    }
}

void Turtle::fireBullet(std::vector<Bullet>& bullets, SDL_Texture* bulletTexture) 
{
    if (!hiding)
    {
        // dang turtles with guns
        if (bulletTimer >= 300)
        {
            if (up || down || left || right)
            {
                int bulletStartX = rect.x + rect.w / 2 - 5;  // make sure the bullets are coming from the turtle
                int bulletStartY = rect.y + rect.h / 2;

                int bulletDx = 0, bulletDy = 0;

                if (up) bulletDy = 1;
                if (down) bulletDy = -1;
                if (left) bulletDx = -1;
                if (right) bulletDx = 1;

                bullets.push_back(Bullet(bulletStartX, bulletStartY, BULLET_SPEED, bulletTexture, bulletDx, bulletDy));
                bulletTimer = 0;  // no fully auto turts
            }
        }
        else
        {
            bulletTimer++;
        }

        for (auto& bullet : bullets)
        {
            bullet.move();
        }

        // erase out of bounds turtle bullets
        bullets.erase(
            remove_if(bullets.begin(), bullets.end(), [](Bullet& b)
                {
                    return b.rect.y < -b.rect.h || b.rect.y > 720 || b.rect.x < 0 || b.rect.x > 1280;
                }),
            bullets.end());
    }
}

void Turtle::hideinShell(SDL_Renderer* renderer, const Turtle& player) 
{
    int deltaX = player.rect.x + player.rect.w / 2 - (rect.x + rect.w / 2);
    int deltaY = player.rect.y + player.rect.h / 2 - (rect.y + rect.h / 2);

    if (deltaX || deltaY <= TURTLE_HIDE_DISTANCE)
    {
        hiding = true;
    }
    else
    {
        hiding = false;
    }
}

void Turtle::spawnTurtles(std::vector<Turtle>& turtles, SDL_Renderer* renderer, int frameCount, SDL_Texture* turtleTexture)
{
    for (auto& turtle : turtles)
    {
        turtle.updateMovement();  // Update movement independent of the player
    }

    if (frameCount % 500 == 0)
    {
        Turtle turtle = { { rand() % (1280 - 50), rand() % (720 - 50), 50, 50 }, false, 0, 0, TURTLE_MOVE_INTERVAL, turtleTexture };
        turtles.push_back(turtle); //bug testing
    }
}
