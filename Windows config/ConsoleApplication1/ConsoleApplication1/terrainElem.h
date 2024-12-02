#pragma once
#include <SDL.h>
#include <vector>
#include <random>
#include "terrain/TerrainGrid.h"

// Forward declare SDL_image functions we need
extern "C" {
    SDL_Texture* IMG_LoadTexture(SDL_Renderer* renderer, const char* file);
    const char* IMG_GetError(void);
}

struct TerrainSprite {
    SDL_Texture* texture;
    SDL_FRect rect;  // Using FRect for more precise positioning
};

class terrainElements {
private:
    std::vector<SDL_Texture*> cattails;
    std::vector<SDL_Texture*> stones;
    std::vector<SDL_Texture*> lilypads;
    std::vector<TerrainSprite> activeSprites;
    SDL_Renderer* renderer;
    TerrainGrid* grid;
    std::mt19937 rng;
    int screenWidth;
    int screenHeight;

    SDL_Point getTextureSize(SDL_Texture *asset);
    void loadTextures();
    SDL_Texture* getRandomTexture(const std::vector<SDL_Texture*>& textures);
    void generateSprites(int count);

public:
    terrainElements(SDL_Renderer* r, TerrainGrid* g, int width, int height);
    ~terrainElements();
    
    void generate(int spriteCount = 100);  // Generate specified number of terrain elements
    void render();
};
