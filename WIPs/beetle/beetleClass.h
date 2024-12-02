#ifndef BEETLE_CLASS_H
#define BEETLE_CLASS_H

#include <SDL.h>
#include "../frog/frogClass.h"
#include "../guns/DefaultShotgun.h"
#include "../healthBar.cpp"

class healthBar;

class BeetleState {
private:
    SDL_Texture* beetleTexture;
    static const int MAX_BEETLES = 20;
    SDL_Rect beetleRect[MAX_BEETLES];
    int beetleCount = MAX_BEETLES;
    int beetleIndex = 0;
    Frog* frog;
    DefaultShotgun* gun;
    SDL_Renderer* renderer;

    bool clearBeetle[MAX_BEETLES];
    healthBar* beetleHpBar[MAX_BEETLES];

    const int beetleMaxHp = 40;
    int beetleWidth = 50;
    int beetleHeight = 50;

    // Damage cooldown variables
    float damageCooldown[MAX_BEETLES];
    const float damageInterval = 1.0f;

    // Spawning variables
    float spawnTimer = 0.0f;
    const float spawnInterval = 5.0f;

public:
    void Init(SDL_Renderer* renderer, Frog* frogInstance, DefaultShotgun* gun);
    void setGun(DefaultShotgun* gun);
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    void CleanUp();
};
#endif BEETLE_CLASS_H