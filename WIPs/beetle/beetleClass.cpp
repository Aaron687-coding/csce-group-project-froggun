#include "beetleClass.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <SDL_image.h>
#include "../healthBar.cpp"

void BeetleState::Init(SDL_Renderer* renderer, Frog* frogInstance, DefaultShotgun* gun)
{
    this->renderer = renderer;
    this->frog = frogInstance;
    this->gun = gun;

    // Load beetle texture
    SDL_Surface* surface = IMG_Load("assets/beetle.png");
    if (!surface) {
        std::cerr << "Failed to load beetle image: " << IMG_GetError() << std::endl;
        return;
    }

    beetleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!beetleTexture) {
        std::cerr << "Failed to create beetle texture from surface!" << std::endl;
        return;
    }

    // Initialize beetles
    for (int i = 0; i < beetleCount; ++i) {
        beetleRect[i] = { 0, 0, beetleWidth, beetleHeight };
        clearBeetle[i] = false;
        beetleHpBar[i] = nullptr;
        damageCooldown[i] = 0.0f;
    }

    std::cout << "BeetleState initialized successfully." << std::endl;
}

void BeetleState::setGun(DefaultShotgun* gun) {
    this->gun = gun;
}

void BeetleState::Update(float deltaTime)
{
    if (frog == nullptr || gun == nullptr) return;

    spawnTimer += deltaTime;
    const float speed = 150.0f;

    // Spawning beetles
    if (spawnTimer >= spawnInterval && beetleIndex < beetleCount) {
        // Randomize starting position
        int startX = rand() % (1280 - beetleWidth);
        int startY = rand() % (720 - beetleHeight);
        beetleRect[beetleIndex] = { startX, startY, beetleWidth, beetleHeight };
        clearBeetle[beetleIndex] = false;
        beetleHpBar[beetleIndex] = new healthBar(renderer, beetleMaxHp);
        damageCooldown[beetleIndex] = 0.0f;

        spawnTimer = 0.0f;
        beetleIndex++;
    }

    for (int i = 0; i < beetleIndex; ++i) {
        if (clearBeetle[i]) continue;

        // Get frog position
        SDL_Rect frogRect = frog->getCollisionBox();

        // Move beetles towards the frog
        int deltaX = frogRect.x - beetleRect[i].x;
        int deltaY = frogRect.y - beetleRect[i].y;
        float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

        if (distance != 0) {
            beetleRect[i].x += static_cast<int>((deltaX / distance) * speed * deltaTime);
            beetleRect[i].y += static_cast<int>((deltaY / distance) * speed * deltaTime);
        }

        // Update damage cooldown
        if (damageCooldown[i] > 0.0f) {
            damageCooldown[i] -= deltaTime;
        }

        // Check collision with frog
        if (SDL_HasIntersection(&beetleRect[i], &frogRect)) {
            if (damageCooldown[i] <= 0.0f) {
                frog->takeDamage(5); // Damage value
                damageCooldown[i] = damageInterval; // Reset cooldown
            }
        }

        // Check for bullet collisions
        if (gun)
        {
            const auto& bullets = gun->getBullets();
            for (const auto& bulletPair : bullets) {
                const auto& bullet = bulletPair.second;
                // Check if bullet hits the abdomen/bottom half
                SDL_Rect abdomenRect = beetleRect[i];
                abdomenRect.y += beetleHeight / 2;
                abdomenRect.h /= 2;

                bool hit = false;
                int damageAmount = bullet.bulletDamage;

                if (SDL_HasIntersection(&bullet.bulletPos, &abdomenRect)) {
                    // Hit the abdomen - double damage
                    damageAmount *= 2;
                    hit = true;
                }
                else if (SDL_HasIntersection(&bullet.bulletPos, &beetleRect[i])) {
                    // Hit elsewhere - normal damage
                    hit = true;
                }

                // If the beetle was hit
                if (hit) {
                    beetleHpBar[i]->damage(damageAmount);

                    // Check if hp is depleted
                    if (beetleHpBar[i]->getHealth() <= 0) {
                        clearBeetle[i] = true;
                        delete beetleHpBar[i];
                        beetleHpBar[i] = nullptr;
                    }

                    break; // Break to prevent multiple hits
                }
            }
        }

        // Update hp bar position
        if (beetleHpBar[i]) {
            beetleHpBar[i]->setPosition(beetleRect[i].x + beetleWidth / 2, beetleRect[i].y);
        }

        // Keep beetles within screen bounds
        if (beetleRect[i].x < 0) beetleRect[i].x = 0;
        if (beetleRect[i].x + beetleRect[i].w > 1280) beetleRect[i].x = 1280 - beetleRect[i].w;
        if (beetleRect[i].y < 0) beetleRect[i].y = 0;
        if (beetleRect[i].y + beetleRect[i].h > 720) beetleRect[i].y = 720 - beetleRect[i].h;
    }
}

void BeetleState::Render(SDL_Renderer* renderer)
{
    if (frog == nullptr || renderer == nullptr) return;

    for (int i = 0; i < beetleIndex; ++i) {
        if (clearBeetle[i]) continue;

        SDL_RenderCopy(renderer, beetleTexture, nullptr, &beetleRect[i]);

        // Render hp bar
        if (beetleHpBar[i]) {
            beetleHpBar[i]->draw();
        }
    }
}

void BeetleState::CleanUp()
{
    if (beetleTexture) {
        SDL_DestroyTexture(beetleTexture);
        beetleTexture = nullptr;
    }

    // Clean up hp bars
    for (int i = 0; i < beetleCount; ++i) {
        if (beetleHpBar[i]) {
            delete beetleHpBar[i];
            beetleHpBar[i] = nullptr;
        }
    }
}