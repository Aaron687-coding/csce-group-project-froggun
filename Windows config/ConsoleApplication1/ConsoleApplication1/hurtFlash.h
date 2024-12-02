#pragma once
#include <SDL.h>
#include <unordered_map>

class hurtFlash {
private:
    static constexpr float flashTime = 0.2f; // in seconds
    std::unordered_map<void*, float> flashingObjects; // Maps object pointer to remaining flash time
    static hurtFlash* instance;

    hurtFlash() {} // Private constructor for singleton

public:
    static hurtFlash* getInstance();
    void update(float deltaTime);
    SDL_Texture* getFilledImage(SDL_Renderer* renderer, SDL_Texture* tex, void* objectPtr);
    void startFlash(void* objectPtr);
};
