#ifndef BAT_CLASS_H
#define BAT_CLASS_H

#include <SDL.h>
#include "../frog/frogClass.h"
#include "../guns/DefaultShotgun.h"
#include "../healthBar.cpp"

class healthBar;

class BatState {
private:
    SDL_Texture* batTexture;
    static const int MAX_BATS = 20;
    SDL_Rect batRect[MAX_BATS];
    int batCount = MAX_BATS;
    SDL_Point batDirections[MAX_BATS];
    Frog* frog;
    float spawnTimer = 0.0f;
    const float spawnInterval = 10.0f;
    int batIndex = 0;

    bool clearBeetle[MAX_BATS];

    DefaultShotgun* gun;
    SDL_Renderer* renderer;

    healthBar* batHpBar[MAX_BATS];

    const int batMaxHp = 10;

public:
    int batWidth = 30;
    int batHeight = 30;

    void Init(SDL_Renderer* renderer, Frog* frogInstance, DefaultShotgun* gun);
    void setGun(DefaultShotgun* gun);
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    void CleanUp();
};

#endif // BAT_CLASS_H