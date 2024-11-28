#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include "frogClass.h"

using namespace std;

// constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float BULLET_SPEED = 1.5;
const int WASP_SPEED = 5;
const int TURTLE_SPEED = 1;
const int TURTLE_HIDE_DISTANCE = 100;
const int TURTLE_MOVE_INTERVAL = 120;

SDL_Texture* loadTexture(const string& path, SDL_Renderer* renderer) 
{
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path.c_str());
    if (newTexture == nullptr) 
    {
        cerr << "Failed to load texture: " << IMG_GetError() << endl;
    }
    return newTexture;
}

// bullets for the turtle
struct Bullet 
{
    SDL_Rect rect;
    SDL_Texture* texture = nullptr;
    float speed = BULLET_SPEED;
    int dx, dy;

    Bullet(int x, int y, int s, SDL_Texture* tex, int dirX, int dirY)
        : rect{ x, y, 50, 50 }, speed(s), dx(dirX), dy(dirY), texture(tex) {}

    void move()
    {
        rect.x += dx * speed;  // Move in the x direction
        rect.y += dy * speed;  // Move in the y direction
    }
};

struct Wasp
{
    SDL_Rect rect;
    SDL_Texture* texture = nullptr;
    int dx, dy;
    int left, right;
    bool active = true;
    bool facingRight = false;

    Wasp(SDL_Rect r, int dx, int dy, SDL_Texture* tex)
        : rect(r), dx(dx), dy(dy), texture(tex), facingRight(false)
    {
        left = right = 0;
    }

    void move()
    {
        rect.x += dx;
        rect.y += dy;
    }

    void moveTowards(const Wasp& player, int speed)
    {
        int deltaX = player.rect.x + player.rect.w / 2 - (rect.x + rect.w / 2);
        int deltaY = player.rect.y + player.rect.h / 2 - (rect.y + rect.h / 2);

        float magnitude = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (magnitude != 0)
        {
            dx = static_cast<int>(speed * deltaX / magnitude);
            dy = static_cast<int>(speed * deltaY / magnitude);
        }

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

    }
};

// gonna burgle your turts
struct Turtle 
{
    SDL_Rect rect;
    SDL_Texture* texture = nullptr;

    int up, down, left, right;
    bool hiding;
    int dx, dy;
    int moveTimer = 0;
    int bulletTimer = 0;
    bool facingRight = false;

    Turtle(SDL_Rect r, bool hiding, int dx, int dy, int moveTimer, SDL_Texture* tex = nullptr)
        : rect(r), hiding(hiding), dx(dx), dy(dy), moveTimer(moveTimer), texture(tex), facingRight(false)
    {
        up = down = left = right = 0;
    }



    void updateMovement()
    {
        if (!hiding)
        {
            if (moveTimer == 0)
            {
                int oldRLPos = dx;
                int oldUDPos = dy;
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

            cout << moveTimer;

            // movement
            rect.x += dx;
            rect.y += dy;

            // no escape
            if (rect.x < 0) rect.x = 0;
            if (rect.y < 0) rect.y = 0;
            if (rect.x + rect.w > SCREEN_WIDTH) rect.x = SCREEN_WIDTH - rect.w;
            if (rect.y + rect.h > SCREEN_HEIGHT) rect.y = SCREEN_HEIGHT - rect.h;
        }
    }
        

    void fireBullet(vector<Bullet>& bullets, SDL_Renderer* renderer, SDL_Texture* bulletTexture)
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
                else
                {
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
                        return b.rect.y < -b.rect.h || b.rect.y > SCREEN_HEIGHT || b.rect.x < 0 || b.rect.x > SCREEN_WIDTH;
                    }),
                bullets.end());

        }

    }
    void hideinShell(SDL_Renderer* renderer, const Turtle& player) 
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
        if (hiding)
        {
            if (texture != loadTexture("assets/shell.png", renderer)) {
                texture = loadTexture("assets/shell.png", renderer);
            }
        }
        else
        {
            if (texture != loadTexture("assets/turtle.png", renderer)) {
                texture = loadTexture("assets/turtle.png", renderer);
            }
        }
    }
};

void spawnTurtles(vector<Turtle>& turtles, vector<Bullet>& bullets, SDL_Renderer* renderer, int frameCount, SDL_Texture* turtleTexture, SDL_Texture* bulletTexture)
{
    for (auto& turtle : turtles)
    {
        turtle.updateMovement();  // Update movement independent of the player
        turtle.fireBullet(bullets, renderer, bulletTexture);  // Fire bullets at regular intervals
    }

    if (frameCount % 500 == 0)
    {
        Turtle turtle = { { rand() % (SCREEN_WIDTH - 50), rand() % (SCREEN_HEIGHT - 50), 50, 50 }, false, 0, 0, TURTLE_MOVE_INTERVAL, turtleTexture };
        turtles.push_back(turtle); //bug testing
        cout << "Spawned Turtle at frame " << frameCount << endl;
    }


}

void spawnWasp(vector<Wasp>& wasps, SDL_Renderer* renderer, int frameCount, SDL_Texture* waspTexture)
{
    if (frameCount % 100 == 0)
    {
        Wasp wasp = { { rand() % (SCREEN_WIDTH - 50), 0, 50, 50 }, 0, 0, waspTexture };
        wasps.push_back(wasp); //bug testing
        cout << "Spawned Wasp at frame " << frameCount << endl;
    }
}

void renderEntities(SDL_Renderer* renderer, const vector<Wasp>& wasps, const vector<Turtle>& turtles, const vector<Bullet>& bullets)
{
    for (const auto& wasp : wasps) 
    {
        SDL_RendererFlip flip = wasp.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx(renderer, wasp.texture, nullptr, &wasp.rect, 0.0, nullptr, flip);
    }

    for (const auto& turtle : turtles) 
    {
        // Render the turtle's texture if it's not hiding. Need to update this with just shell png
        if (turtle.texture) 
        {
            SDL_RendererFlip flip = turtle.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            SDL_RenderCopyEx(renderer, turtle.texture, nullptr, &turtle.rect, 0.0, nullptr, flip);
        }
        else 
        {
            // if the textures refuse to load again
            cout << "Turt textures aren't loading properly again";
        }
    }

    for (const auto& bullet : bullets)
    {
        if (bullet.texture != nullptr)  // Check if bullet has a valid texture
        {
            SDL_RenderCopy(renderer, bullet.texture, nullptr, &bullet.rect);
        }
        else
        {
            cout << "Bullet texture is null!" << endl;
        }
    }
}

int main(int argc, char* argv[]) 
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("FROGGUN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* waspTexture = loadTexture("assets/wasp.png", renderer);
    SDL_Texture* turtleTexture = loadTexture("assets/turtle.png", renderer);
    SDL_Texture* bulletTexture = loadTexture("assets/orb_blue.png", renderer);
    if (bulletTexture == nullptr) {
        cout << "Failed to load bullet texture: " << IMG_GetError() << endl;
    }

    vector<Wasp> wasps;
    vector<Turtle> turtles;
    vector<Bullet> bullets;

    Frog player(400, 500); //wasps need this

    int frameCount = 0;
    bool quit = false;
    SDL_Event e;

    while (!quit) 
    {
        while (SDL_PollEvent(&e) != 0) 
        {
            if (e.type == SDL_QUIT) 
            {
                quit = true;
            }
        }

        frameCount++;

        // spawn enemies
        spawnWasp(wasps, renderer, frameCount, waspTexture);
        spawnTurtles(turtles, bullets, renderer, frameCount, turtleTexture, bulletTexture);

        // clear screen for new frame
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // renders mobs
        renderEntities(renderer, wasps, turtles, bullets);

        
        SDL_RenderPresent(renderer);

        SDL_Delay(16);  // this should cap fps to 60
    }

    // clean up time
    SDL_DestroyTexture(waspTexture);
    SDL_DestroyTexture(turtleTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}