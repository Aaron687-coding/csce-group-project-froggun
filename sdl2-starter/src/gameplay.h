/*********************************************
Author: Aaron Chakine
Description: The main loop, which runs game states using a state manager from the GameStateManager class. 
             See GameStateManager.h for state manager details and GameState.h for state details.

Subsequent changes:
Format: [Author] - [Changes]
- Added terrain generation and menu state with simple text rendering
- Added SDL_ttf for font rendering

Memory & Health Bar Fixes (2024):
1. Added proper copy semantics to healthBar class to prevent memory corruption during vector operations
2. Added visibility control to health bars to prevent render issues
3. Added pendingRemoval state to entities to properly handle cleanup
4. Modified entity spawn functions to use reserve() and emplace_back for better memory management
5. Added proper renderer management through currentRenderer member
6. Fixed health bar initialization timing to ensure proper setup before use
7. Added proper state handling for entity removal to prevent premature cleanup
8. Fixed render command conflicts by properly managing health bar visibility states
9. Improved entity lifecycle management with better state transitions
10. Added safety checks for renderer availability in Update and Render methods

- added hurtFlash header and implementation files to show damage
*********************************************/

#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "GameState.h"
#include "frog/frogClass.h"
#include "turtle/turtleStruct.h"
#include "wasp/waspStruct.h"
#include "guns/DefaultShotgun.h"
#include "terrain/TerrainGrid.h"
#include "terrainElem.h"
#include "RainSystem.h"
#include "waterPhysics.h"
#include "hurtFlash.h"
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
    SDL_Renderer* currentRenderer;  // Add renderer member

    // Damage constants
    const int WASP_DAMAGE = 10;
    const int BULLET_DAMAGE = 30;
    const int FROG_MAX_HEALTH = 100;

    // How many enemies are spawned at a time
    const int numTurtlesSpawned = 1;
    const int numWaspsSpawned = 3;

    // Add hurtFlash instance
    hurtFlash* flashManager;

    Frog frog;
    SDL_Texture* spritesheet;
    SDL_Texture* tongueTip;  // Added for tongue rendering
    // Define frog states as constants
    const Frog::State frogIdle = Frog::State::IDLE;
    const Frog::State frogGrappling = Frog::State::GRAPPLING;
    const Frog::State frogJumping = Frog::State::JUMPING;
    const Frog::State frogFalling = Frog::State::FALLING;
    const Frog::State frogDead = Frog::State::DEAD;

    // Wasp and Turtle textures
    SDL_Texture* turtleTexture;
    SDL_Texture* shellTexture;  // Added separate texture for shell
    SDL_Texture* bulletTexture;
    SDL_Texture* waspTexture;

    // Member vectors to hold active wasps and turtles
    std::vector<Wasp> wasps;
    std::vector<Turtle> turtles;
    std::vector<Bullet> bullets;

    std::shared_ptr<TerrainGrid> terrain;
    std::shared_ptr<terrainElements> terrainElems;
    std::unique_ptr<RainSystem> rainSystem;
    std::unique_ptr<WaterPhysics> waterPhysics;  // Added water physics system

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

    void checkBulletCollisions(SDL_Renderer* renderer) {
        auto& shotgunBullets = shotgun->getBullets();
        
        // Check wasp collisions
        for (auto& wasp : wasps) {
            if (!wasp.active) continue;
            
            SDL_Rect waspBox = wasp.rect;
            for (const auto& [id, bullet] : shotgunBullets) {
                if (SDL_HasIntersection(&bullet.bulletPos, &waspBox)) {
                    wasp.takeDamage(bullet.bulletDamage);
                    flashManager->startFlash(&wasp); // Start flash effect
                    break;
                }
            }
        }
        
        // Check turtle collisions
        for (auto& turtle : turtles) {
            if (turtle.hiding) continue;
            
            SDL_Rect turtleBox = turtle.rect;
            for (const auto& [id, bullet] : shotgunBullets) {
                if (SDL_HasIntersection(&bullet.bulletPos, &turtleBox)) {
                    turtle.takeDamage(bullet.bulletDamage);
                    flashManager->startFlash(&turtle); // Start flash effect
                    break;
                }
            }
        }
    }

    void checkEnemyCollisions() {
        SDL_Rect frogBox = frog.getCollisionBox();

        // Check wasp collisions
        for (auto& wasp : wasps) {
            if (!wasp.active || wasp.pendingRemoval) continue;
            
            if (SDL_HasIntersection(&wasp.rect, &frogBox) && wasp.canDealDamage()
                && frog.getState() != Frog::State::JUMPING) {
                frog.takeDamage(WASP_DAMAGE);
                flashManager->startFlash(&frog); // Start flash effect
                wasp.resetDamageTimer();
            }
        }
    }

    void updateWasps(std::vector<Wasp>&wasps, Frog & player, int speed) {
        for (auto it = wasps.begin(); it != wasps.end(); )
        {
            if (!it->active || it->pendingRemoval) {
                // Ensure health bar is properly cleaned up before removal
                if (it->health) {
                    delete it->health;
                    it->health = nullptr;
                }
                it = wasps.erase(it);
            }
            else {
                it->moveTowards(player, speed);
                it->updateHealthBar();
                it->updateDamageTimer(1.0f/60.0f); // Update damage cooldown timer
                ++it;
            }
        }
    }

    void updateTurtles() {
        for (auto it = turtles.begin(); it != turtles.end(); ) {
            if (it->pendingRemoval) {
                // Ensure health bar is properly cleaned up before removal
                if (it->health) {
                    delete it->health;
                    it->health = nullptr;
                }
                it = turtles.erase(it);
            } else {
                it->hideinShell(frog);
                it->updateMovement();
                it->updateHealthBar();
                it->fireBullet(bullets, frog, 1.0f/60.0f, currentRenderer, bulletTexture);
                ++it;
            }
        }
    }

    void updateBullets(std::vector<Bullet>&bullets, Frog & player) {
        SDL_Rect frogBox = frog.getCollisionBox();

        for (auto it = bullets.begin(); it != bullets.end(); ) {
            SDL_Rect bulletBox = it->rect;
            // Check if the bullet's rectangle intersects with the frog's rectangle
            if (SDL_HasIntersection(&bulletBox, &frogBox)) {
                // Apply damage to the frog when hit by a bullet
                frog.takeDamage(BULLET_DAMAGE);
                flashManager->startFlash(&frog); // Start flash effect
                it = bullets.erase(it);  // Remove the bullet after dealing damage
            }
            else {
                ++it; // Continue to the next bullet
            }
        }
    }

public:
    gameplay() : frog(1280.0f / 2, 720.0f / 2), spritesheet(nullptr), tongueTip(nullptr), 
                turtleTexture(nullptr), shellTexture(nullptr), bulletTexture(nullptr), 
                waspTexture(nullptr), shotgun(nullptr), currentRenderer(nullptr) {
        rainSystem = std::make_unique<RainSystem>(SCREEN_WIDTH, SCREEN_HEIGHT);
        flashManager = hurtFlash::getInstance();
    }

    void setTerrain(std::shared_ptr<TerrainGrid> t) {
        terrain = t;
    }

    void setTerrainElements(std::shared_ptr<terrainElements> te) {
        terrainElems = te;
    }

    void Init() override {
        // We'll load the spritesheet in the first Render call since we need the renderer
    }

    void HandleEvents(SDL_Event& event) override {
        // Don't handle events if frog is dead
        if (frog.getState() == Frog::State::DEAD) return;

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
                shotgun->reload();
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
                !keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]
                 && frog.getState() != Frog::State::GRAPPLING) {
                frog.stopMoving();
            }
        }
    }

    void Update(float deltaTime) override {
        if (!currentRenderer) return;  // Skip update if renderer isn't ready

        // Update flash effects
        flashManager->update(deltaTime);

        // Update rain
        if (rainSystem) {
            rainSystem->update(deltaTime);
        }

        // Update water physics and check if frog is on water
        if (waterPhysics && terrain) {
            waterPhysics->update(deltaTime, *terrain);
            
            // Check if frog is on water and update its state
            SDL_Rect frogBox = frog.getCollisionBox();
            int gridX = frogBox.x / terrain->getCellSize();
            int gridY = frogBox.y / terrain->getCellSize();
            bool isOnWater = terrain->isWater(gridX, gridY);
            
            // Update frog's water state
            frog.setOnWater(isOnWater);
            
            // Add water ring at frog's position if on water
            if (isOnWater) {
                waterPhysics->addFrogRing(frogBox.x + frogBox.w/2, frogBox.y + frogBox.h/2);
            }
        }
        
        // Update frog's position and state
        frog.update(deltaTime);
        
        // Check enemy collisions if frog is alive
        if (frog.getState() != Frog::State::DEAD) {
            checkEnemyCollisions();
        }
        
        // Update shotgun and check for bullet collisions
        if (shotgun) {
            shotgun->update(deltaTime);
            shotgun->updateBullets();
            checkBulletCollisions(currentRenderer);
        }

        static int frameCount = 0; // Track frame count to control spawning
        frameCount++;

        // Spawn turtles and wasps once every few frames
        for (int i = 0; i < numTurtlesSpawned; i++)
            Turtle::spawnTurtles(turtles, frameCount, turtleTexture, currentRenderer, 0);
        // Spawn several wasps at a time
        for (int i = 0; i < numWaspsSpawned; i++)
            Wasp::spawnWasps(wasps, frameCount, waspTexture, currentRenderer);
        
        // Update turt bullets
        updateBullets(bullets, frog);
        
        // Update wasps
        updateWasps(wasps, frog, 3);

        // Update turtles
        updateTurtles();
    }

    void Render(SDL_Renderer* renderer) override {
        currentRenderer = renderer;  // Store renderer for use in Update

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
                frog.addAnimation(frogDead, spritesheet, 16, 14, 1, 0);
                
                // Initialize frog's health bar
                frog.initializeHealthBar(renderer, FROG_MAX_HEALTH);
            }

            // Load mob textures if they are not already loaded
            if (!waspTexture) {
                waspTexture = loadTexture("assets/wasp.png", renderer);
                if (waspTexture) {
                    SDL_SetTextureBlendMode(waspTexture, SDL_BLENDMODE_BLEND);
                }
            }
            if (!turtleTexture) {
                turtleTexture = loadTexture("assets/turtle.png", renderer);
            }
            if (!shellTexture) {
                shellTexture = loadTexture("assets/shell.png", renderer);
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
            
            // Create terrain elements if not provided
            if (!terrainElems) {
                terrainElems = std::make_shared<terrainElements>(renderer, terrain.get(), SCREEN_WIDTH, SCREEN_HEIGHT);
                terrainElems->generate();
            }

            // Initialize water physics system
            if (!waterPhysics) {
                waterPhysics = std::make_unique<WaterPhysics>(renderer);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render terrain first as background
        if (terrain) {
            terrain->render(renderer);
        }
        
        // Render terrain elements
        if (terrainElems) {
            terrainElems->render();
        }

        // Render water effects after terrain but before entities
        if (waterPhysics) {
            waterPhysics->render(renderer);
        }

        // Render rain after water effects but before entities
        if (rainSystem) {
            rainSystem->render(renderer);
        }

        // Get the current animation frame and texture
        SDL_Rect srcRect = frog.getCurrentFrame();
        SDL_Rect destRect = frog.getCollisionBox();
        SDL_Texture* currentTexture = frog.getCurrentTexture();
        
        if (currentTexture) {
            // Flip the texture based on the direction the frog is facing
            SDL_RendererFlip flip = (frog.getFacing() == Frog::Direction::LEFT) ? 
                                   SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            
            // Get potentially flashing texture
            SDL_Texture* displayTexture = flashManager->getFilledImage(renderer, currentTexture, &frog);
            
            // Render the frog
            SDL_RenderCopyEx(renderer, displayTexture, &srcRect, &destRect, 
                           0.0, nullptr, flip);
            
            // Clean up if a new texture was created
            if (displayTexture != currentTexture) {
                SDL_DestroyTexture(displayTexture);
            }
            
            // Draw the frog's health bar
            frog.drawHealthBar();
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

        // Render wasps and turtles
        for (auto& wasp : wasps) 
        {
            if (!wasp.pendingRemoval) {  // Only render if not pending removal
                SDL_RendererFlip flip = (wasp.facingRight) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                SDL_Texture* displayTexture = flashManager->getFilledImage(renderer, wasp.texture, &wasp);
                SDL_RenderCopyEx(renderer, displayTexture, nullptr, &wasp.rect, 0.0, nullptr, flip);
                if (displayTexture != wasp.texture) {
                    SDL_DestroyTexture(displayTexture);
                }
                wasp.renderHealthBar(renderer);
            }
        }

        // Render turtles with flash effect
        for (auto& turtle : turtles) {
            if (!turtle.pendingRemoval) {
                SDL_Texture* baseTexture = turtle.hiding ? shellTexture : turtleTexture;
                SDL_Texture* displayTexture = flashManager->getFilledImage(renderer, baseTexture, &turtle);
                SDL_RendererFlip flip = (turtle.facingRight) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                SDL_RenderCopyEx(renderer, displayTexture, nullptr, &turtle.rect, 0.0, nullptr, flip);
                if (displayTexture != baseTexture) {
                    SDL_DestroyTexture(displayTexture);
                }
                turtle.renderHealthBar(renderer);
            }
        }

        // Render bullets (if any)
        for (const auto& bullet : bullets) 
        {
            SDL_RenderCopy(renderer, bulletTexture, nullptr, &bullet.rect);
        }

        if (!waspTexture) {
            SDL_Log("Failed to load wasp texture: %s", IMG_GetError());
        }

        if (!turtleTexture) {
            SDL_Log("Failed to load turtle texture: %s", IMG_GetError());
        }

        // Finally, render the shotgun
        // Render bullet trails and shells
        if (shotgun) {
            shotgun->render(renderer, destRect.x + destRect.w/2, destRect.y + destRect.h/2);
        }
    }

    void CleanUp() override {
        currentRenderer = nullptr;  // Clear renderer reference
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
        if (turtleTexture) {
            SDL_DestroyTexture(turtleTexture);
            turtleTexture = nullptr;
        }
        if (shellTexture) {
            SDL_DestroyTexture(shellTexture);
            shellTexture = nullptr;
        }
        if (bulletTexture) {
            SDL_DestroyTexture(bulletTexture);
            bulletTexture = nullptr;
        }
        if (waspTexture) {
            SDL_DestroyTexture(waspTexture);
            waspTexture = nullptr;
        }
    }
};

#endif // GAMEPLAY_H
