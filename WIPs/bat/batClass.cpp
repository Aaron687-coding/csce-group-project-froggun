#include "batClass.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <SDL_image.h>
#include "../healthBar.cpp"

void BatState::Init(SDL_Renderer* renderer, Frog* frogInstance, DefaultShotgun* gun)
{
    this->renderer = renderer;
    this->frog = frogInstance;
    this->gun = gun;

    // Load bat texture
    SDL_Surface* surface = IMG_Load("assets/bat.png");
    if (!surface) {
        std::cerr << "Failed to load bat image: " << IMG_GetError() << std::endl;
        return;
    }

    batTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!batTexture) {
        std::cerr << "Failed to create bat texture from surface!" << std::endl;
        return;
    }

    for (int i = 0; i < batCount; ++i) {
        batRect[i] = { 0, 0, batWidth, batHeight };
        batDirections[i] = { 0, 0 };
        clearBeetle[i] = false;
        batHpBar[i] = nullptr;
    }

    std::cout << "BatState initialized successfully." << std::endl;
}

void BatState::setGun(DefaultShotgun* gun)
{
    this->gun = gun;
}

void BatState::Update(float deltaTime)
{
    if (frog == nullptr || gun == nullptr) return;

    spawnTimer += deltaTime;

    if (spawnTimer >= spawnInterval && batIndex < batCount) {
        // Random starting position
        int startX = rand() % (1280 - batWidth);
        int startY = rand() % (720 - batHeight);
        batRect[batIndex] = { startX, startY, batWidth, batHeight };

        // Random movement direction
        int directionX = rand() % 3 - 1;
        int directionY = rand() % 3 - 1;

        if (directionX == 0 && directionY == 0) {
            directionX = 1;
        }

        batDirections[batIndex] = { directionX, directionY };
        clearBeetle[batIndex] = false;

        // Initialize hp bar
        batHpBar[batIndex] = new healthBar(renderer, batMaxHp);
        batHpBar[batIndex]->setPosition(startX + batWidth / 2, startY);

        spawnTimer = 0.0f;
        batIndex++;
    }

    const float speed = 200.0f;

    for (int i = 0; i < batIndex; ++i) {
        if (clearBeetle[i]) continue;

        // Move bats
        batRect[i].x += static_cast<int>(batDirections[i].x * speed * deltaTime);
        batRect[i].y += static_cast<int>(batDirections[i].y * speed * deltaTime);

        // Bounce off walls
        if (batRect[i].x < 0 || batRect[i].x + batRect[i].w > 1280) batDirections[i].x *= -1;
        if (batRect[i].y < 0 || batRect[i].y + batRect[i].h > 720) batDirections[i].y *= -1;

        // Update hp bar position
        if (batHpBar[i]) {
            batHpBar[i]->setPosition(batRect[i].x + batWidth / 2, batRect[i].y);
        }

        // Check for bullet collisions
        if (gun) {
            const auto& bullets = gun->getBullets();

            // Loop through each bullet in the bullets container
            for (const auto& bulletPair : bullets) {
                // Extract the bullet object from the pair
                const auto& bullet = bulletPair.second;
                if (SDL_HasIntersection(&bullet.bulletPos, &batRect[i])) {
                    batHpBar[i]->damage(bullet.bulletDamage);

                    // Check hp for less than or equal to 0
                    if (batHpBar[i]->getHealth() <= 0) {
                        clearBeetle[i] = true;
                        delete batHpBar[i];
                        batHpBar[i] = nullptr;
                    }
                    break;
                }
            }
        }

        // Check frog collision
        SDL_Rect frogRect = frog->getCollisionBox();
        if (SDL_HasIntersection(&batRect[i], &frogRect))
        {
            frog->takeDamage(5); // Damage to frog
        }
    }
}

void BatState::Render(SDL_Renderer* renderer)
{
    if (frog == nullptr || renderer == nullptr) return;

    for (int i = 0; i < batIndex; ++i) {
        if (clearBeetle[i]) continue;

        SDL_RenderCopy(renderer, batTexture, nullptr, &batRect[i]);

        // Draw hp bar
        if (batHpBar[i]) {
            batHpBar[i]->draw();
        }
    }
}

void BatState::CleanUp()
{
    if (batTexture) {
        SDL_DestroyTexture(batTexture);
        batTexture = nullptr;
    }

    // Clean up hp bars
    for (int i = 0; i < batCount; ++i) {
        if (batHpBar[i]) {
            delete batHpBar[i];
            batHpBar[i] = nullptr;
        }
    }
}