#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <random>

class TerrainGrid {
private:
    int width;
    int height;
    int cellSize;
    SDL_Color waterColor;
    SDL_Color swampColor;
    SDL_Color grassColor;
    float waterThreshold;
    float grassThreshold;
    std::vector<std::vector<float>> grid;
    SDL_Texture* terrainTexture;
    SDL_Renderer* renderer;
    bool needsUpdate;
    uint32_t seed;
    std::mt19937 rng;

    // Perlin noise helper functions
    float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
    float lerp(float a, float b, float t) { return a + t * (b - a); }
    float grad(int hash, float x, float y);
    float noise(float x, float y);
    float octaveNoise(float x, float y, int octaves, float persistence);
    std::vector<int> p; // Permutation table
    void initPermutationTable();

public:
    TerrainGrid(SDL_Renderer* renderer, int width, int height, int cellSize);
    ~TerrainGrid();
    void setColors(SDL_Color water, SDL_Color swamp, SDL_Color grass);
    void setWaterThreshold(float threshold) { waterThreshold = threshold; needsUpdate = true; }
    void setGrassThreshold(float threshold) { grassThreshold = threshold; needsUpdate = true; }
    float getWaterThreshold() const { return waterThreshold; }
    float getGrassThreshold() const { return grassThreshold; }
    int getCellSize() const { return cellSize; }
    float getValueAt(int x, int y) const { return grid[y][x]; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isWater(int x, int y) const { return getValueAt(x, y) < waterThreshold; }
    void generate();
    void render(SDL_Renderer* renderer);
};
