#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "GameState.h"
#include "frogClass.h"
#include <SDL2/SDL_render.h>

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
            // Only allow movement if the frog is not grounded (jumping or grappling)
            if (!frog.getGrounded()) {
                int xDir = 0;
                int yDir = 0;

                if (keys[SDL_SCANCODE_W]) {
                    yDir = -1; // Go up (negative, since the top is zero for y)
                }
                if (keys[SDL_SCANCODE_S]) {
                    yDir = 1; // Go down
                }
                if (keys[SDL_SCANCODE_A]) {
                    xDir = -1; // Go left (negative, since the upper left corner is (0, 0))
                }
                if (keys[SDL_SCANCODE_D]) {
                    xDir = 1; // Go right
                }

                if (xDir != 0 || yDir != 0) {
                    frog.move(xDir, yDir); // move frog if movement is nonzero
                }
            }

            // Handle jumping with space - can only jump when grounded
            if (keys[SDL_SCANCODE_SPACE] && frog.getGrounded()) {
                frog.jump();
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
            // bright pink, same as the tongue_tip.png image
            SDL_SetRenderDrawColor(renderer, 255, 161, 229, 255);
                                            //   ^ r  ^ g  ^ b  ^ alpha
            
            // Calculate tongue start position (frog's mouth)
            int startX = destRect.x + destRect.w/2;
            int startY = destRect.y + destRect.h/2;
            
            // Get mouse position for tongue end
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            // Draw multiple lines for thickness
            for(int i = -6; i <= 6; i++) {
                SDL_RenderDrawLine(renderer, 
                    startX + i, startY,  // Offset start point for thickness
                    mouseX + i, mouseY   // Offset end point for thickness
                );
            }
            
            // Render tongue tip
            if (tongueTip) {
                SDL_Rect tipRect = {
                    mouseX - 8,  // Center the 16x16 tip
                    mouseY - 8,
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
