#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "GameState.h"
#include "frogClass.h"
#include "GunTemplate.h"
#include <SDL2/SDL_render.h>
#include <cmath>

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

public:
    gameplay() : frog(1280.0f / 2, 720.0f / 2), spritesheet(nullptr), tongueTip(nullptr) {} // Initialize frog in constructor

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

            int startX = destRect.x + destRect.w/2 + (xOff); // fine-tune the mouth with xOff and 4
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
            const int NUM_LINES = 32; // Increased for smoother appearance
            const float MAX_OFFSET = 6.0f;
            
            for(int i = 0; i < NUM_LINES; i++) {
                // Use cosine distribution for smoother density
                float t = (i / (float)(NUM_LINES - 1)) * M_PI;
                float offset = MAX_OFFSET * std::cos(t);

                // Smooth color transition
                float colorBlend = std::abs(offset) / MAX_OFFSET;
                if (colorBlend > 0.67f) {
                    // outline color (warm brown)
                    SDL_SetRenderDrawColor(renderer, 154, 76, 0, 255);
                } else {
                    // bright pink
                    SDL_SetRenderDrawColor(renderer, 255, 161, 229, 255);
                }

                // Calculate offset points using perpendicular vector
                int startOffsetX = startX + static_cast<int>(perpX * offset);
                int startOffsetY = startY + static_cast<int>(perpY * offset);
                int endOffsetX = frog.getGrappleX() + static_cast<int>(perpX * offset);
                int endOffsetY = frog.getGrappleY() + static_cast<int>(perpY * offset);

                // Draw slightly thicker line for better coverage
                for(int j = 0; j < 2; j++) {
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
    }
};

#endif // GAMEPLAY_H
