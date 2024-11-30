#ifndef WASP_H
#define WASP_H

#include <SDL2/SDL.h>
#include <vector>
#include "../frog/frogClass.h"
#include "../healthBar.cpp"
#include <SDL2/SDL_image.h>
using namespace std;

struct Wasp 
{
    SDL_Rect rect;
    SDL_Texture* texture;
    float dx, dy;
    int left, right;
    bool active;
    bool facingRight;
    healthBar* health;
    bool pendingRemoval;  // New flag to handle delayed removal

    Wasp(SDL_Rect r, float dx, float dy, SDL_Texture* tex);

    // Copy constructor
    Wasp(const Wasp& other) : 
        rect(other.rect), texture(other.texture), dx(other.dx), dy(other.dy),
        left(other.left), right(other.right), active(other.active),
        facingRight(other.facingRight), health(nullptr), pendingRemoval(other.pendingRemoval) {
        if (other.health) {
            health = new healthBar(*other.health);
        }
    }

    // Move constructor
    Wasp(Wasp&& other) noexcept :
        rect(other.rect), texture(other.texture), dx(other.dx), dy(other.dy),
        left(other.left), right(other.right), active(other.active),
        facingRight(other.facingRight), health(other.health), pendingRemoval(other.pendingRemoval) {
        other.health = nullptr;  // Transfer ownership
        other.texture = nullptr;
    }

    // Copy assignment operator
    Wasp& operator=(const Wasp& other) {
        if (this != &other) {
            rect = other.rect;
            texture = other.texture;
            dx = other.dx;
            dy = other.dy;
            left = other.left;
            right = other.right;
            active = other.active;
            facingRight = other.facingRight;
            pendingRemoval = other.pendingRemoval;

            // Handle health bar
            delete health;
            health = other.health ? new healthBar(*other.health) : nullptr;
        }
        return *this;
    }

    // Move assignment operator
    Wasp& operator=(Wasp&& other) noexcept {
        if (this != &other) {
            rect = other.rect;
            texture = other.texture;
            dx = other.dx;
            dy = other.dy;
            left = other.left;
            right = other.right;
            active = other.active;
            facingRight = other.facingRight;
            pendingRemoval = other.pendingRemoval;

            // Handle health bar
            delete health;
            health = other.health;
            other.health = nullptr;
            other.texture = nullptr;
        }
        return *this;
    }
    
    void initHealthBar(SDL_Renderer* renderer) {
        if (!health) {
            health = new healthBar(renderer, 20);
        }
    }

    void takeDamage(int amount) {
        if (health) {
            health->damage(amount);
            if (health->getHealth() <= 0) {
                pendingRemoval = true;  // Mark for removal instead of immediate deactivation
                if (health) {
                    health->setVisible(false);  // Hide the health bar
                }
            }
        }
    }

    void updateHealthBar() {
        if (health && active && !pendingRemoval) {
            health->setPosition(rect.x + rect.w/2, rect.y);
        }
    }

    void renderHealthBar(SDL_Renderer* renderer) {
        if (health && active && !pendingRemoval) {
            health->draw();
        }
    }

    bool shouldRemove() const {
        return pendingRemoval;
    }

    ~Wasp() {
        delete health;
    }
    
    void moveTowards(Frog& player, int speed);
    static void spawnWasps(vector<Wasp>& wasps, int frameCount, SDL_Texture* waspTexture, SDL_Renderer* renderer);
};

#endif
