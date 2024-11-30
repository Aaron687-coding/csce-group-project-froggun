#pragma once
#include <SDL2/SDL.h>
#include <deque>
#include <random>
#include <ctime>

struct RainDrop {
    float x, y;           // Position
    float velocityY;      // Vertical velocity
    float velocityX;      // Horizontal velocity
    float lifetime;       // How long the drop exists
    Uint8 alpha;         // Transparency
};

class RainSystem {
private:
    std::deque<RainDrop> raindrops;
    std::mt19937 rng;
    int screenWidth;
    int screenHeight;
    static const int MAX_DROPS = 500;
    
public:
    RainSystem(int width, int height) : screenWidth(width), screenHeight(height) {
        rng.seed(std::time(nullptr));
    }

    void update(float deltaTime) {
        // Add new raindrops
        std::uniform_real_distribution<float> xDist(0, static_cast<float>(screenWidth));
        
        // Add 2 new drops per frame if we haven't reached max
        for (int i = 0; i < 2 && raindrops.size() < MAX_DROPS; i++) {
            RainDrop drop;
            drop.x = xDist(rng);
            drop.y = -5.0f;  // Start above screen
            drop.velocityY = 500.0f;  // Falling speed
            drop.velocityX = -150.f; // Make the rain fall slightly slanted
            drop.lifetime = 2.0f;     // Live for 2 seconds
            drop.alpha = 180;         // Slightly transparent
            raindrops.push_back(drop);
        }

        // Update existing drops
        for (auto it = raindrops.begin(); it != raindrops.end();) {
            it->y += it->velocityY * deltaTime;
            it->x += it->velocityX * deltaTime;
            it->lifetime -= deltaTime;
            it->alpha = static_cast<Uint8>((it->lifetime / 2.0f) * 180);

            // Remove drops that are off screen or expired
            if (it->lifetime <= 0 || it->y > screenHeight) {
                it = raindrops.erase(it);
            } else {
                ++it;
            }
        }
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        
        for (const auto& drop : raindrops) {
            // Set color to light blue with alpha
            SDL_SetRenderDrawColor(renderer, 173, 216, 230, drop.alpha);
            
            // Draw raindrop as a small line
            SDL_RenderDrawLine(renderer,
                static_cast<int>(drop.x),
                static_cast<int>(drop.y),
                static_cast<int>(drop.x + drop.velocityX / 60),
                static_cast<int>(drop.y + 10));  // 10 pixels long
        }
    }
};
