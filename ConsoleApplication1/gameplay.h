#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "GameState.h"
#include "frogClass.h"
#include "GunTemplate.h"
#include <SDL_render.h>
#include <SDL_image.h>
#include <cmath>
#include <vector>
#include "waspStruct.h"
#include "turtleStruct.h"
#include "bulletStruct.h"

class gameplay : public GameState {
private:
    Frog frog;
    SDL_Texture* spritesheet;
    SDL_Texture* tongueTip;  // Added for tongue rendering
    // Define frog states as constants
    const Frog::State frogIdle = Frog::State::IDLE;
    const Frog::State frogGrappling = Frog::State::GRAPPLING;
    const Frog::State frogJumping = Frog::State::JUMPING;
    const Frog::State frogFalling = Frog::State::FALLING;

    // Wasp and Turtle textures
    SDL_Texture* turtleTexture;
    SDL_Texture* shell;
    SDL_Texture* bulletTexture;
    SDL_Texture* waspTexture;

    // Member vectors to hold active wasps and turtles
    vector<Wasp> wasps;
    vector<Turtle> turtles;
    vector<Bullet> bullets;

public:
    gameplay() : frog(1280.0f / 2, 720.0f / 2), spritesheet(nullptr), tongueTip(nullptr), turtleTexture(nullptr), shell(nullptr), bulletTexture(nullptr), waspTexture(nullptr) {} // Initialize frog in constructor

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
        }

        // Handle mouse events for grappling
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            frog.grapple(mouseX, mouseY);
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

        static int frameCount = 0; // Track frame count to control spawning
        frameCount++;

        // Spawn turtles and wasps only once every few frames
        Turtle::spawnTurtles(turtles, frameCount, turtleTexture);
        Wasp::spawnWasps(wasps, frameCount, waspTexture);

        // Move each wasp towards the frog
        for (auto& wasp : wasps)
        {
            wasp.moveTowards(frog, 3); // Move each wasp towards the frog
            wasp.move();
        }

        //adds collision for the wasps and frog
        void updateWasps(vector<Wasp>&wasps, Frog & player, int speed);
        {

            for (auto it = wasps.begin(); it != wasps.end(); )
            {
                it->moveTowards(frog, 1);

                if (!it->active)
                {

                    it = wasps.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        void updateBullets(vector<Bullet>&bullets, Frog & player);
        {
            SDL_Rect frogBox = frog.getCollisionBox();
            for (auto it = bullets.begin(); it != bullets.end(); )
            {
                SDL_Rect bulletBox = it->rect;

                // Check if the bullet's rectangle intersects with the frog's rectangle
                if (SDL_HasIntersection(&bulletBox, &frogBox))
                {
                    // Bullet hits the frog, remove the bullet from the vector
                    it = bullets.erase(it);

                }
                else
                {
                    ++it; // Continue to the next bullet
                }
            }
        }

    



        for (auto& turtle : turtles) 
        {
            turtle.hideinShell(frog);
            turtle.updateMovement(); 
            turtle.fireBullet(bullets, frog, deltaTime, nullptr, nullptr);

        }

        // Get the current collision box
        SDL_Rect frogBox = frog.getCollisionBox();

        // Keep the frog within screen bounds
        if (frogBox.x < 0) frogBox.x = 0;
        if (frogBox.y < 0) frogBox.y = 0;
        if (frogBox.x + frogBox.w > 1280) frogBox.x = 1280 - frogBox.w;
        if (frogBox.y + frogBox.h > 720) frogBox.y = 720 - frogBox.h;
    }

    void Render(SDL_Renderer* renderer) override {
        // Load the spritesheet if it hasn't been loaded yet
        if (!spritesheet || !tongueTip) {
            // ASSET LOADING - CHANGE ASSETS HERE
            spritesheet = IMG_LoadTexture(renderer, "assets/frog.png");
            tongueTip = IMG_LoadTexture(renderer, "assets/tongue_tip.png");
            turtleTexture = IMG_LoadTexture(renderer, "assets/turtle.png");
            waspTexture = IMG_LoadTexture(renderer, "assets/wasp.png");
            shell = IMG_LoadTexture(renderer, "assets/shell.png");
            bulletTexture = IMG_LoadTexture(renderer, "assets/bulletNew.png");
            if (!spritesheet || !tongueTip) {
                SDL_Log("Failed to load texture: %s", IMG_GetError());
            }
            if (spritesheet) {
                frog.addAnimation(frogIdle, spritesheet, 16, 14, 1, 0);
                frog.addAnimation(frogGrappling, spritesheet, 16, 14, 1, 0);
                frog.addAnimation(frogJumping, spritesheet, 16, 14, 1, 0);
                frog.addAnimation(frogFalling, spritesheet, 16, 14, 1, 0);
            }
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

            int startX = destRect.x + destRect.w / 2 + (xOff); // fine-tune the mouth with xOff and 4
            int startY = destRect.y + destRect.h / 2 + 4;

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
            const int NUM_LINES = 32; // Increased for smoother appearance
            const float MAX_OFFSET = 6.0f;

            for (int i = 0; i < NUM_LINES; i++) {
                // Use cosine distribution for smoother density
                float t = (i / (float)(NUM_LINES - 1)) * M_PI;
                float offset = MAX_OFFSET * std::cos(t);

                // Smooth color transition
                float colorBlend = std::abs(offset) / MAX_OFFSET;
                if (colorBlend > 0.67f) {
                    // outline color (warm brown)
                    SDL_SetRenderDrawColor(renderer, 154, 76, 0, 255);
                }
                else {
                    // bright pink
                    SDL_SetRenderDrawColor(renderer, 255, 161, 229, 255);
                }

                // Calculate offset points using perpendicular vector
                int startOffsetX = startX + static_cast<int>(perpX * offset);
                int startOffsetY = startY + static_cast<int>(perpY * offset);
                int endOffsetX = frog.getGrappleX() + static_cast<int>(perpX * offset);
                int endOffsetY = frog.getGrappleY() + static_cast<int>(perpY * offset);

                // Draw slightly thicker line for better coverage
                for (int j = 0; j < 2; j++) {
                    SDL_RenderDrawLine(renderer,
                        startOffsetX + j, startOffsetY,
                        endOffsetX + j, endOffsetY);
                }
            }

            // Render tongue tip
            if (tongueTip) {
                SDL_Rect tipRect = {
                    static_cast<int>(frog.getGrappleX()) - 8,  // Center the 16x16 tip
                    static_cast<int>(frog.getGrappleY()) - 8,
                    16, 16
                };
                SDL_RenderCopy(renderer, tongueTip, nullptr, &tipRect);
            }

            
        }

        // Render wasps and turtles
        for (const auto& wasp : wasps) 
        {
            SDL_RendererFlip flip = (wasp.facingRight) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderCopyEx(renderer, wasp.texture, nullptr, &wasp.rect, 0.0, nullptr, flip);
        }

        for (const auto& turtle : turtles)
        {
            
            if (turtle.hiding)
            {
                turtleTexture = IMG_LoadTexture(renderer, "assets/shell.png"); 
            }

            else
            {
                turtleTexture = IMG_LoadTexture(renderer, "assets/turtle.png");
            }
            SDL_RendererFlip flip = (turtle.facingRight) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderCopyEx(renderer, turtleTexture, nullptr, &turtle.rect, 0.0, nullptr, flip);

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
        if (turtleTexture) SDL_DestroyTexture(turtleTexture);
        if (shell) SDL_DestroyTexture(shell);
        if (bulletTexture) SDL_DestroyTexture(bulletTexture);
        if (waspTexture) SDL_DestroyTexture(waspTexture);
    }
};

#endif // GAMEPLAY_H