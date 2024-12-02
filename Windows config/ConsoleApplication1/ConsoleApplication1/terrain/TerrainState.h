#pragma once
#include "../GameState.h"
#include "TerrainGrid.h"
#include <memory>

class TerrainState : public GameState {
private:
    std::unique_ptr<TerrainGrid> terrain;
    bool regenerateRequested;
    bool initialized;

public:
    TerrainState() : initialized(false) {}

    void Init() override {
        regenerateRequested = false;
        initialized = false;
    }

    void Update(float deltaTime) override {
        if (regenerateRequested) {
            terrain->generate();
            regenerateRequested = false;
        }
    }

    void Render(SDL_Renderer* renderer) override {
        if (!initialized) {
            // Create a 64x36 grid with 20px cells (1280x720 window)
            terrain = std::make_unique<TerrainGrid>(renderer, 32, 18, 40);
            terrain->generate();
            initialized = true;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        terrain->render(renderer);
    }

    void CleanUp() override {
        terrain.reset();
    }

    void requestRegeneration() { regenerateRequested = true; }
};
