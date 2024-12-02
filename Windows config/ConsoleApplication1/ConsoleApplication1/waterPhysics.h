#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "terrain/TerrainGrid.h"
#include <random>
#include <ctime>
#include <vector>

struct WaterRing {
    float x, y;           // Position
    float scale;          // Current scale
    float alpha;          // Current transparency
    float lifetime;       // Current lifetime
    bool isSmall;        // Whether this is a small ring or regular ring
    
    WaterRing(float x, float y, bool small) 
        : x(x), y(y), scale(0.3f), alpha(1.0f), lifetime(0.0f), isSmall(small) {}
};

class WaterPhysics {
private:
    SDL_Texture* waterRingTexture;
    SDL_Texture* smallWaterRingTexture;
    std::vector<WaterRing> activeRings;
    std::mt19937 rng;
    float spawnTimer;
    float frogRingTimer;  // Added timer for frog ring spawning
    const float SPAWN_INTERVAL = 0.1f;  // Increased spawn frequency (reduced from 0.5f to 0.1f)
    const float FROG_RING_INTERVAL = 0.2f;  // Added interval for frog ring spawning
    
public:
    WaterPhysics(SDL_Renderer* renderer);
    ~WaterPhysics();
    
    void addFrogRing(float x, float y);
    void update(float deltaTime, const TerrainGrid& terrain);
    void render(SDL_Renderer* renderer);
};
