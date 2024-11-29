#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "GameState.h"
#include "frog/frogClass.h"
#include "turtle/turtleStruct.h"
#include "wasp/waspStruct.h"
#include "guns/DefaultShotgun.h"
#include "terrain/TerrainGrid.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <memory>

// Forward declare SDL_image functions we need
extern "C" {
    SDL_Texture* IMG_LoadTexture(SDL_Renderer* renderer, const char* file);
    const char* IMG_GetError(void);
}

class gameplay : public GameState {
private:
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    // Consult with your timer for frame count now :D
    float timer = 0;
    int waspCounter = 0;
    int turtleCounter = 0;
    const int WASP_SPAWN_RATE = 2; // Every 2 seconds
    const int TURTLE_SPAWN_RATE = 5; // Every five seconds
    const int WASP_CHASE_SPEED = 2; // Speed at which wasps chase the frog

    Frog frog;
    SDL_Texture* spritesheet;
    SDL_Texture* tongueTip;  // Added for tongue rendering
    // Mob textures
    SDL_Texture* waspTexture;
    SDL_Texture* turtleTexture;
    SDL_Texture* bulletTexture;
    // Define frog states as constants
    const Frog::State frogIdle = Frog::State::IDLE;
    const Frog::State frogGrappling = Frog::State::GRAPPLING;
    const Frog::State frogJumping = Frog::State::JUMPING;
    const Frog::State frogFalling = Frog::State::FALLING;

    std::shared_ptr<TerrainGrid> terrain;

    // Lazily copy Fisher's functions into the class
    void renderEntities(SDL_Renderer* renderer, const std::vector<Wasp>& wasps, const std::vector<Turtle>& turtles, const std::vector<Bullet>& bullets)
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
                std::cout << "Turt textures aren't loading properly again";
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
                std::cout << "Bullet texture is null!" << std::endl;
            }
        }
    }

    void spawnTurtles(std::vector<Turtle>& turtles, std::vector<Bullet>& bullets, SDL_Texture* turtleTexture, SDL_Texture* bulletTexture)
    {
        for (auto& turtle : turtles)
        {
            turtle.updateMovement();  // Update movement independent of the player
            // Remove renderer so that we can call it in update
            turtle.fireBullet(bullets, bulletTexture);  // Fire bullets at regular intervals
        }

        Turtle turtle = { { rand() % (SCREEN_WIDTH - 50), rand() % (SCREEN_HEIGHT - 50), 50, 50 }, false, 0, 0, turtle.TURTLE_MOVE_INTERVAL, turtleTexture };
        turtles.push_back(turtle); //bug testing
    }

    void spawnWasp(std::vector<Wasp>& wasps, SDL_Texture* waspTexture)
    {
        Wasp wasp = { { rand() % (SCREEN_WIDTH - 50), 0, 50, 50 }, 0, 0, waspTexture };
        wasps.push_back(wasp); //bug testing
    }

    DefaultShotgun* shotgun;

    // Fisher's method for loading textures
    SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) 
    {
        SDL_Texture* newTexture = IMG_LoadTexture(renderer, path.c_str());
        if (newTexture == nullptr) 
        {
            std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl;
        }
        return newTexture;
    }


public:
    gameplay() : frog(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f), spritesheet(nullptr), tongueTip(nullptr), shotgun(nullptr) {} // Initialize frog in constructor

    std::vector<Wasp> wasps;
    std::vector<Turtle> turtles;
    std::vector<Bullet> bullets;

    void setTerrain(std::shared_ptr<TerrainGrid> t) {
        terrain = t;
    }

    void Init() override {
        // We'll load the spritesheet in the first Render call since we need the renderer
    }

    void HandleEvents(SDL_Event& event) override {
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        
        if (event.type == SDL_KEYDOWN) {
            // Initialize movement variables with zero
            int xDir = 0;
            int yDir = 0;

            if (keys[SDL_SCANCODE_W]) {
                yDir = 1; // Go up
            }
            if (keys[SDL_SCANCODE_S]) {
                yDir = -1; // Go down
            }
            if (keys[SDL_SCANCODE_A]) {
                xDir = 1; // Go left
            }
            if (keys[SDL_SCANCODE_D]) {
                xDir = -1; // Go right
            }

            // Handle jumping with space - can only jump when grounded
            if (keys[SDL_SCANCODE_SPACE] && frog.getGrounded()) {
                frog.jump(static_cast<float>(xDir), static_cast<float>(yDir));
            }

            // Handle reload with R key
            if (keys[SDL_SCANCODE_R] && shotgun) {
                shotgun->reload();  // This will handle both state change and reload logic
            }
        }
        
        // Handle mouse events for grappling and shooting
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            // Right click to grapple
            if (event.button.button == SDL_BUTTON_RIGHT) {
                frog.grapple(mouseX, mouseY);
            // Left click to shoot
            } else if (event.button.button == SDL_BUTTON_LEFT && shotgun) {
                // Get frog position for shooting
                SDL_Rect frogBox = frog.getCollisionBox();
                shotgun->shoot(frogBox.x + frogBox.w/2, frogBox.y + frogBox.h/2, mouseX, mouseY);
            }
        }
        
        // Stop movement when keys are released
        if (event.type == SDL_KEYUP) {
            if (!keys[SDL_SCANCODE_W] && !keys[SDL_SCANCODE_S] && 
                !keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]) {
                frog.stopMoving();
            }
        }
    }

    void Update(float deltaTime) override {
        // Update frog's position and state
        frog.update(deltaTime);
        
        // Update shotgun
        if (shotgun) {
            shotgun->update(deltaTime);
            shotgun->updateBullets();
        }
        
        // Get the current collision box
        SDL_Rect frogBox = frog.getCollisionBox();
        
        // Keep the frog within screen bounds
        if (frogBox.x < 0) frogBox.x = 0;
        if (frogBox.y < 0) frogBox.y = 0;
        if (frogBox.x + frogBox.w > SCREEN_WIDTH) frogBox.x = SCREEN_WIDTH - frogBox.w;
        if (frogBox.y + frogBox.h > SCREEN_HEIGHT) frogBox.y = SCREEN_HEIGHT - frogBox.h;

        // Update wasps to chase the frog
        for (auto& wasp : wasps) {
            // Create a temporary wasp struct with the frog's position to use with moveTowards
            Wasp frogTarget({frogBox.x, frogBox.y, frogBox.w, frogBox.h}, 0, 0, nullptr);
            wasp.moveTowards(frogTarget, WASP_CHASE_SPEED);
            wasp.facingRight = wasp.rect.x < frogBox.x; // Update facing direction based on frog position
        }

        // Update turtles and make them shoot at the frog
        for (auto& turtle : turtles) {
            turtle.updateMovement();
            turtle.facingRight = turtle.rect.x < frogBox.x; // Update facing direction based on frog position
            turtle.fireBullet(bullets, bulletTexture); // Shoot bullets at the frog
        }

        timer += deltaTime;
        if (timer > 1.0f) {
            timer -= 1.0f;
            waspCounter++;
            turtleCounter++;

            if (waspCounter == WASP_SPAWN_RATE) {
                waspCounter = 0;
                spawnWasp(wasps, waspTexture);
            }
            if (turtleCounter == TURTLE_SPAWN_RATE) {
                turtleCounter = 0;
                spawnTurtles(turtles, bullets, turtleTexture, bulletTexture);
            }
        }
    }
        

    void Render(SDL_Renderer* renderer) override {
        // Load the spritesheet if it hasn't been loaded yet
        if (!spritesheet || !tongueTip) {
            //ASSET LOADING - CHANGE ASSETS HERE
            spritesheet = IMG_LoadTexture(renderer, "assets/frog.png");
            tongueTip = IMG_LoadTexture(renderer, "assets/tongue_tip.png");
            if (!spritesheet || !tongueTip) {
                SDL_Log("Failed to load texture: %s", IMG_GetError());
            }
            if (spritesheet) {
                frog.addAnimation(frogIdle, spritesheet, 16, 14, 1, 0);
                frog.addAnimation(frogGrappling, spritesheet, 16, 14, 1, 0);
                frog.addAnimation(frogJumping, spritesheet, 16, 14, 1, 0);
                frog.addAnimation(frogFalling, spritesheet, 16, 14, 1, 0);
            }

            // Load mob textures if they are not already loaded
            if (!waspTexture) {
                waspTexture = loadTexture("assets/wasp.png", renderer);
            }
            if (!turtleTexture) {
                turtleTexture = loadTexture("assets/turtle.png", renderer);
            }
            if (!bulletTexture) {
                bulletTexture = loadTexture("assets/bulletNew.png", renderer);
            }
            if (bulletTexture == nullptr) {
                std::cout << "Failed to load bullet texture: " << IMG_GetError() << std::endl;
            }
            
            // Initialize shotgun after renderer is available
            if (!shotgun) {
                shotgun = new DefaultShotgun(renderer);
            }

            // Create terrain if not provided
            if (!terrain) {
                terrain = std::make_shared<TerrainGrid>(renderer, 64, 36, 20);
                terrain->generate();
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render terrain first as background
        if (terrain) {
            terrain->render(renderer);
        }

        // Get the current animation frame and texture
        SDL_Rect srcRect = frog.getCurrentFrame();
        SDL_Rect destRect = frog.getCollisionBox();
        SDL_Texture* currentTexture = frog.getCurrentTexture();
        
        if (currentTexture) {
            // Flip the texture based on the direction the frog is facing
            SDL_RendererFlip flip = (frog.getFacing() == Frog::Direction::LEFT) ? 
                                   SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            
            // Render the frog
            SDL_RenderCopyEx(renderer, currentTexture, &srcRect, &destRect, 
                           0.0, nullptr, flip);
        }

        if (frog.getState() == Frog::State::GRAPPLING) {
            // Calculate tongue start position (frog's mouth)
            int xOff;

            // Make sure the tongue always comes from the center of the mouth
            if (frog.getFacing() == Frog::Direction::RIGHT)
                xOff = 10;
            else
                xOff = -10;

            int startX = destRect.x + destRect.w/2 + (xOff);
            int startY = destRect.y + destRect.h/2 + 4;
            
            // Get mouse position for tongue end
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Calculate direction vector
            float dirX = mouseX - startX;
            float dirY = mouseY - startY;

            // Calculate distance
            float distance = std::sqrt(dirX * dirX + dirY * dirY);

            // Normalize direction vector
            float normalizedDirX = dirX / distance;
            float normalizedDirY = dirY / distance;

            // Calculate perpendicular vector (rotate 90 degrees)
            float perpX = -normalizedDirY;
            float perpY = normalizedDirX;
            
            // Draw multiple lines for thickness with smaller spacing
            const int NUM_LINES = 32;
            const float MAX_OFFSET = 6.0f;
            
            for(int i = 0; i < NUM_LINES; i++) {
                float t = (i / (float)(NUM_LINES - 1)) * M_PI;
                float offset = MAX_OFFSET * std::cos(t);

                float colorBlend = std::abs(offset) / MAX_OFFSET;
                if (colorBlend > 0.67f) {
                    SDL_SetRenderDrawColor(renderer, 154, 76, 0, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, 161, 229, 255);
                }

                int startOffsetX = startX + static_cast<int>(perpX * offset);
                int startOffsetY = startY + static_cast<int>(perpY * offset);
                int endOffsetX = frog.getGrappleX() + static_cast<int>(perpX * offset);
                int endOffsetY = frog.getGrappleY() + static_cast<int>(perpY * offset);

                for(int j = 0; j < 2; j++) {
                    SDL_RenderDrawLine(renderer, 
                        startOffsetX + j, startOffsetY,
                        endOffsetX + j, endOffsetY);
                }
            }
            
            // Render tongue tip
            if (tongueTip) {
                SDL_Rect tipRect = {
                    static_cast<int>(frog.getGrappleX()) - 8,
                    static_cast<int>(frog.getGrappleY()) - 8,
                    16, 16
                };
                SDL_RenderCopy(renderer, tongueTip, nullptr, &tipRect);
            }
        }
        // Render enemies
        renderEntities(renderer, wasps, turtles, bullets);

        // Finally, render the shotgun
        // Render bullet trails and shells
        if (shotgun) {
            shotgun->render(renderer, destRect.x + destRect.w/2, destRect.y + destRect.h/2);
        }
        
    }

    void CleanUp() override {
        if (spritesheet) {
            SDL_DestroyTexture(spritesheet);
            spritesheet = nullptr;
        }
        if (tongueTip) {
            SDL_DestroyTexture(tongueTip);
            tongueTip = nullptr;
        }
        if (shotgun) {
            delete shotgun;
            shotgun = nullptr;
        }

        if (waspTexture) {
            delete waspTexture;
            waspTexture = nullptr;
        }
        if (turtleTexture) {
            delete turtleTexture;
            turtleTexture = nullptr;
        }
        if (bulletTexture) {
            delete bulletTexture;
            bulletTexture = nullptr;
        }
    }
};

#endif // GAMEPLAY_H
