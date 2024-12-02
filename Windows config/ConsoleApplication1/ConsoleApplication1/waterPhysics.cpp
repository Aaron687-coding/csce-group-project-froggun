#include "waterPhysics.h"

WaterPhysics::WaterPhysics(SDL_Renderer* renderer) {
    // Load textures
    waterRingTexture = IMG_LoadTexture(renderer, "assets/waterRing.png");
    smallWaterRingTexture = IMG_LoadTexture(renderer, "assets/smallWaterRing.png");
    
    // Initialize random number generator
    rng.seed(std::time(nullptr));
    spawnTimer = 0.0f;
    frogRingTimer = 0.0f;
}

WaterPhysics::~WaterPhysics() {
    SDL_DestroyTexture(waterRingTexture);
    SDL_DestroyTexture(smallWaterRingTexture);
}

void WaterPhysics::addFrogRing(float x, float y) {
    // Only add a ring if enough time has passed
    if (frogRingTimer >= FROG_RING_INTERVAL) {
        // Randomly decide whether to spawn a ring
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        if (dist(rng) < 0.3f) {  // 30% chance to spawn a ring
            activeRings.emplace_back(x, y, false);
        }
        frogRingTimer = 0.0f;
    }
}

void WaterPhysics::update(float deltaTime, const TerrainGrid& terrain) {
    // Update timers
    spawnTimer += deltaTime;
    frogRingTimer += deltaTime;
    
    // Update existing rings
    for (auto it = activeRings.begin(); it != activeRings.end();) {
        it->lifetime += deltaTime;
        
        // Scale up over time
        it->scale += deltaTime * (it->isSmall ? 1.5f : 3.0f);
        
        // Fade out over time
        it->alpha = std::max(0.0f, 1.0f - (it->lifetime / 1.0f));
        
        // Remove rings that have completed their animation
        if (it->lifetime >= 1.0f) {
            it = activeRings.erase(it);
        } else {
            ++it;
        }
    }
    
    // Spawn random rain rings on water tiles
    if (spawnTimer >= SPAWN_INTERVAL) {
        spawnTimer = 0.0f;
        
        // Get grid dimensions
        int gridWidth = terrain.getWidth();
        int gridHeight = terrain.getHeight();
        
        // Try to spawn multiple rings per interval
        for (int i = 0; i < 150; i++) {  // Attempt to spawn up to 150 rings per interval
            std::uniform_int_distribution<int> distX(0, gridWidth - 1);
            std::uniform_int_distribution<int> distY(0, gridHeight - 1);
            
            int x = distX(rng);
            int y = distY(rng);
            
            // If the selected tile is water, spawn a ring with 50% chance
            if (terrain.isWater(x, y)) {
                std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
                if (chanceDist(rng) < 0.5f) {  // 50% chance to spawn
                    float worldX = x * terrain.getCellSize();
                    float worldY = y * terrain.getCellSize();
                    activeRings.emplace_back(worldX, worldY, true);
                }
            }
        }
    }
}

void WaterPhysics::render(SDL_Renderer* renderer) {
    for (const auto& ring : activeRings) {
        SDL_Texture* texture = ring.isSmall ? smallWaterRingTexture : waterRingTexture;
        
        // Get texture dimensions
        int texWidth, texHeight;
        SDL_QueryTexture(texture, nullptr, nullptr, &texWidth, &texHeight);
        
        // Calculate destination rectangle
        int size = static_cast<int>(texWidth * ring.scale);
        SDL_Rect dstRect = {
            static_cast<int>(ring.x - size/2),
            static_cast<int>(ring.y - size/2),
            size,
            size
        };
        
        // Set alpha for transparency
        SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(255 * ring.alpha));
        
        // Render the ring
        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    }
}
