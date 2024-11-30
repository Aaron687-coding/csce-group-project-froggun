#include "terrainElem.h"
#include <random>
#include <chrono>

terrainElements::terrainElements(SDL_Renderer* r, TerrainGrid* g, int width, int height)
    : renderer(r), grid(g), screenWidth(width), screenHeight(height) {
    // Initialize RNG with time-based seed
    rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    loadTextures();
}

terrainElements::~terrainElements() {
    // Cleanup textures using SDL_DestroyTexture
    for (auto tex : cattails) SDL_DestroyTexture(tex);
    for (auto tex : stones) SDL_DestroyTexture(tex);
    for (auto tex : lilypads) SDL_DestroyTexture(tex);
}

SDL_Point terrainElements::getTextureSize(SDL_Texture* asset) {
    SDL_Point size;
    SDL_QueryTexture(asset, nullptr, nullptr, &size.x, &size.y);
    return size;
}

void terrainElements::loadTextures() {
    // Load cattail textures
    cattails.push_back(IMG_LoadTexture(renderer, "assets/terrain/cattail1.png"));
    cattails.push_back(IMG_LoadTexture(renderer, "assets/terrain/cattail2.png"));
    cattails.push_back(IMG_LoadTexture(renderer, "assets/terrain/cattail3.png"));

    // Load stone textures
    stones.push_back(IMG_LoadTexture(renderer, "assets/terrain/stone1.png"));
    stones.push_back(IMG_LoadTexture(renderer, "assets/terrain/stone2.png"));
    stones.push_back(IMG_LoadTexture(renderer, "assets/terrain/stone3.png"));
    
    

    // Load lilypad textures
    lilypads.push_back(IMG_LoadTexture(renderer, "assets/terrain/lilypad1.png"));
    lilypads.push_back(IMG_LoadTexture(renderer, "assets/terrain/lilypad2.png"));
    lilypads.push_back(IMG_LoadTexture(renderer, "assets/terrain/lilypad3.png"));
}

SDL_Texture* terrainElements::getRandomTexture(const std::vector<SDL_Texture*>& textures) {
    std::uniform_int_distribution<int> dist(0, textures.size() - 1);
    return textures[dist(rng)];
}

void terrainElements::generateSprites(int count) {
    std::uniform_real_distribution<float> xDist(0, screenWidth);
    std::uniform_real_distribution<float> yDist(0, screenHeight);

    activeSprites.clear();
    
    for (int i = 0; i < count; i++) {
        float x = xDist(rng);
        float y = yDist(rng);
        
        // Get grid cell coordinates
        int gridX = static_cast<int>(x / grid->getCellSize());
        int gridY = static_cast<int>(y / grid->getCellSize());
        
        // Get the noise value at this position
        float value = grid->getValueAt(gridX, gridY);
        
        SDL_Texture* selectedTexture = nullptr;
        
        // Select appropriate texture based on terrain type
        if (value < grid->getWaterThreshold()) {
            selectedTexture = getRandomTexture(lilypads);
        } else if (value < grid->getGrassThreshold()) {
            selectedTexture = getRandomTexture(cattails);
        } else {
            selectedTexture = getRandomTexture(stones);
        }
        
        if (selectedTexture) {
            SDL_Point texSize = getTextureSize(selectedTexture);
            
            TerrainSprite sprite;
            sprite.texture = selectedTexture;
            sprite.rect = {
                x - texSize.x * 3 / 2,
                y - texSize.y * 3 / 2,
                static_cast<float>(texSize.x * 3),
                static_cast<float>(texSize.y * 3)
            };
            activeSprites.push_back(sprite);
        }
    }
}

void terrainElements::generate(int spriteCount) {
    generateSprites(spriteCount);
}

void terrainElements::render() {
    for (const auto& sprite : activeSprites) {
        SDL_RenderCopyF(renderer, sprite.texture, nullptr, &sprite.rect);
    }
}
