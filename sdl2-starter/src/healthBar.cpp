#pragma once
#include <SDL2/SDL.h>

class healthBar {
private:
    int MAX_HEALTH;
    int health;
    SDL_Renderer* renderer;
    float x, y; // Position for drawing the health bar
    bool isVisible;

public:
    healthBar(SDL_Renderer* rend, int maxHealth = 100) {
        renderer = rend;
        MAX_HEALTH = maxHealth;
        health = MAX_HEALTH;
        x = 0;
        y = 0;
        isVisible = true;
    }

    // Copy constructor
    healthBar(const healthBar& other) {
        MAX_HEALTH = other.MAX_HEALTH;
        health = other.health;
        renderer = other.renderer;
        x = other.x;
        y = other.y;
        isVisible = other.isVisible;
    }

    // Move constructor
    healthBar(healthBar&& other) noexcept {
        MAX_HEALTH = other.MAX_HEALTH;
        health = other.health;
        renderer = other.renderer;
        x = other.x;
        y = other.y;
        isVisible = other.isVisible;
        
        // Clear the source object
        other.renderer = nullptr;
        other.health = 0;
        other.isVisible = false;
    }

    // Assignment operator
    healthBar& operator=(const healthBar& other) {
        if (this != &other) {
            MAX_HEALTH = other.MAX_HEALTH;
            health = other.health;
            renderer = other.renderer;
            x = other.x;
            y = other.y;
            isVisible = other.isVisible;
        }
        return *this;
    }

    // Move assignment operator
    healthBar& operator=(healthBar&& other) noexcept {
        if (this != &other) {
            MAX_HEALTH = other.MAX_HEALTH;
            health = other.health;
            renderer = other.renderer;
            x = other.x;
            y = other.y;
            isVisible = other.isVisible;
            
            // Clear the source object
            other.renderer = nullptr;
            other.health = 0;
            other.isVisible = false;
        }
        return *this;
    }

    ~healthBar() {
        renderer = nullptr; // Don't delete renderer, it's managed elsewhere
    }

    void setMaxHealth(int newHealth) {
        MAX_HEALTH = newHealth;
        health = MAX_HEALTH;
    }

    void setHealth(int newHealth) {
        health = (newHealth > MAX_HEALTH) ? MAX_HEALTH : newHealth;
        if (health < 0) health = 0;
    }

    void damage(int amount) {
        health -= amount;
        if (health < 0) health = 0;
        // Don't hide the health bar here, let the entity manage visibility
    }

    int getHealth() const {
        return health;
    }

    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    void setVisible(bool visible) {
        isVisible = visible;
    }

    bool getVisible() const {
        return isVisible;
    }

    void draw() {
        if (!renderer || !isVisible) return;
        
        const int BAR_WIDTH = 50;
        const int BAR_HEIGHT = 5;
        const int OFFSET_Y = -20; // Draw above the character

        // Background (empty health bar)
        SDL_Rect bgRect = {
            static_cast<int>(x - BAR_WIDTH/2),
            static_cast<int>(y + OFFSET_Y),
            BAR_WIDTH,
            BAR_HEIGHT
        };
        SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
        SDL_RenderFillRect(renderer, &bgRect);

        // Foreground (current health)
        int currentWidth = static_cast<int>((float)health / MAX_HEALTH * BAR_WIDTH);
        SDL_Rect healthRect = {
            static_cast<int>(x - BAR_WIDTH/2),
            static_cast<int>(y + OFFSET_Y),
            currentWidth,
            BAR_HEIGHT
        };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &healthRect);
    }
};
