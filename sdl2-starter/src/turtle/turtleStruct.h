#ifndef TURTLE_H
#define TURTLE_H

#include <SDL2/SDL.h>
#include <vector>
#include "turtBullet/bulletStruct.h"
#include "../frog/frogClass.h"
#include "../healthBar.cpp"

using namespace std;

struct Turtle
{
    SDL_Rect rect;
    SDL_Texture* texture;
    int up, down, left, right;
    bool hiding;
    float dx, dy;
    int bulletTimer;
    bool facingRight;
    int turtmoveTimer;     
    int moveDuration;
    healthBar* health;
    bool pendingRemoval;  // New flag to handle delayed removal
    static int turtCounter;

    Turtle(SDL_Rect r, bool hiding, float dx, float dy, SDL_Texture* tex);

    // Copy constructor
    Turtle(const Turtle& other) : 
        rect(other.rect), texture(other.texture), up(other.up), down(other.down),
        left(other.left), right(other.right), hiding(other.hiding), dx(other.dx),
        dy(other.dy), bulletTimer(other.bulletTimer), facingRight(other.facingRight),
        turtmoveTimer(other.turtmoveTimer), moveDuration(other.moveDuration),
        health(nullptr), pendingRemoval(other.pendingRemoval) {
        if (other.health) {
            health = new healthBar(*other.health);
        }
    }

    // Move constructor
    Turtle(Turtle&& other) noexcept :
        rect(other.rect), texture(other.texture), up(other.up), down(other.down),
        left(other.left), right(other.right), hiding(other.hiding), dx(other.dx),
        dy(other.dy), bulletTimer(other.bulletTimer), facingRight(other.facingRight),
        turtmoveTimer(other.turtmoveTimer), moveDuration(other.moveDuration),
        health(other.health), pendingRemoval(other.pendingRemoval) {
        other.health = nullptr;  // Transfer ownership
        other.texture = nullptr;
    }

    // Copy assignment operator
    Turtle& operator=(const Turtle& other) {
        if (this != &other) {
            rect = other.rect;
            texture = other.texture;
            up = other.up;
            down = other.down;
            left = other.left;
            right = other.right;
            hiding = other.hiding;
            dx = other.dx;
            dy = other.dy;
            bulletTimer = other.bulletTimer;
            facingRight = other.facingRight;
            turtmoveTimer = other.turtmoveTimer;
            moveDuration = other.moveDuration;
            pendingRemoval = other.pendingRemoval;

            // Handle health bar
            delete health;
            health = other.health ? new healthBar(*other.health) : nullptr;
        }
        return *this;
    }

    // Move assignment operator
    Turtle& operator=(Turtle&& other) noexcept {
        if (this != &other) {
            rect = other.rect;
            texture = other.texture;
            up = other.up;
            down = other.down;
            left = other.left;
            right = other.right;
            hiding = other.hiding;
            dx = other.dx;
            dy = other.dy;
            bulletTimer = other.bulletTimer;
            facingRight = other.facingRight;
            turtmoveTimer = other.turtmoveTimer;
            moveDuration = other.moveDuration;
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
            health = new healthBar(renderer, 50);
        }
    }

    void takeDamage(int amount) {
        if (health) {
            health->damage(amount);
            if (health->getHealth() <= 0) {
                pendingRemoval = true;  // Mark for removal instead of immediate hiding
                if (health) {
                    health->setVisible(false);  // Hide the health bar
                }
                hiding = true;  // Hide in shell when health is depleted
            }
        }
    }

    void updateHealthBar() {
        if (health && !hiding && !pendingRemoval) {
            health->setPosition(rect.x + rect.w/2, rect.y);
        }
    }

    void renderHealthBar(SDL_Renderer* renderer) {
        if (health && !hiding && !pendingRemoval) {
            health->draw();
        }
    }

    bool shouldRemove() const {
        return pendingRemoval;
    }

    ~Turtle() {
        delete health;
    }

    void updateMovement();
    void fireBullet(vector<Bullet>& bullets, Frog& player, float deltaTime, SDL_Renderer* renderer, SDL_Texture* bulletTexture);
    void hideinShell(Frog& player);
    static void spawnTurtles(vector<Turtle>& turtles, int frameCount, SDL_Texture* turtleTexture, SDL_Renderer* renderer, int maxTurts);
};

#endif
