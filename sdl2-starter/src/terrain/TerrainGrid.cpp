#include "TerrainGrid.h"
#include <cmath>
#include <chrono>
#include <iostream>
#include <random>

TerrainGrid::TerrainGrid(SDL_Renderer* r, int w, int h, int cs) 
    : renderer(r), width(w), height(h), cellSize(cs), waterThreshold(0.425f), grassThreshold(0.55f), needsUpdate(true) {
    
    // Initialize default colors
    waterColor = {8, 143, 143, 255};    // Blue green
    swampColor = {64, 181, 173, 255};   // Greener blue green
    grassColor = {111, 210, 144, 255}; // GREEN 

    p.resize(512);
    grid.resize(height, std::vector<float>(width));
    
    // Create texture for caching
    terrainTexture = SDL_CreateTexture(renderer,
                                     SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_TARGET,
                                     width * cellSize,
                                     height * cellSize);
                                     
    // Initialize RNG with hardware random device
    std::random_device rd;
    seed = rd();
    rng.seed(seed);
    
    // Generate initial terrain
    generate();
}

TerrainGrid::~TerrainGrid() {
    if (terrainTexture) {
        SDL_DestroyTexture(terrainTexture);
    }
}

void TerrainGrid::initPermutationTable() {
    std::vector<int> permutation(256);
    for(int i = 0; i < 256; i++) {
        permutation[i] = i;
    }
    std::shuffle(permutation.begin(), permutation.end(), rng);

    for(int i = 0; i < 256; i++) {
        p[i] = permutation[i];
        p[256 + i] = permutation[i];
    }
}

float TerrainGrid::grad(int hash, float x, float y) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float TerrainGrid::noise(float x, float y) {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);

    float u = fade(x);
    float v = fade(y);

    int A = p[X] + Y;
    int B = p[X + 1] + Y;

    return lerp(
        lerp(grad(p[A], x, y),
             grad(p[B], x - 1, y),
             u),
        lerp(grad(p[A + 1], x, y - 1),
             grad(p[B + 1], x - 1, y - 1),
             u),
        v);
}

float TerrainGrid::octaveNoise(float x, float y, int octaves, float persistence) {
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;

    for(int i = 0; i < octaves; i++) {
        total += noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }

    return total / maxValue;
}

void TerrainGrid::setColors(SDL_Color water, SDL_Color swamp, SDL_Color grass) {
    waterColor = water;
    swampColor = swamp;
    grassColor = grass;
    needsUpdate = true;
}

void TerrainGrid::generate() {
    std::cout << "Generating new terrain..." << std::endl;

    // Create a new random device for each generation
    std::random_device rd;
    std::mt19937 localRng(rd());
    
    // Generate a completely new seed
    std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
    seed = dist(localRng);
    rng.seed(seed);
    
    std::cout << "New seed: " << seed << std::endl;

    // Generate random rotation angle and offsets
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> offsetDist(-1000.0f, 1000.0f);
    
    float angle = angleDist(rng);
    float offsetX = offsetDist(rng);
    float offsetY = offsetDist(rng);
    
    std::cout << "Angle: " << angle << ", Offset X: " << offsetX << ", Offset Y: " << offsetY << std::endl;

    // Generate new permutation table
    initPermutationTable();

    float scale = 0.05f;
    int octaves = 6;
    float persistence = 0.5f;

    // Rotation matrix
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            // Apply rotation and offset
            float rotX = (x * cosAngle - y * sinAngle + offsetX) * scale;
            float rotY = (x * sinAngle + y * cosAngle + offsetY) * scale;
            
            float value = octaveNoise(rotX, rotY, octaves, persistence);
            value = (value + 1.0f) * 0.5f;
            
            grid[y][x] = value;
        }
    }

    // Destroy and recreate texture
    if (terrainTexture) {
        SDL_DestroyTexture(terrainTexture);
    }
    terrainTexture = SDL_CreateTexture(renderer,
                                     SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_TARGET,
                                     width * cellSize,
                                     height * cellSize);
    
    needsUpdate = true;
    std::cout << "Terrain generation complete." << std::endl;
}

void TerrainGrid::render(SDL_Renderer* renderer) {
    if (needsUpdate) {
        std::cout << "Updating terrain texture..." << std::endl;
        
        // Set render target to our texture
        SDL_SetRenderTarget(renderer, terrainTexture);
        
        // Clear the texture first
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        SDL_Rect cell = {0, 0, cellSize, cellSize};

        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                cell.x = x * cellSize;
                cell.y = y * cellSize;
                
                float value = grid[y][x];
                
                if(value < waterThreshold) {
                    SDL_SetRenderDrawColor(renderer, waterColor.r, waterColor.g, waterColor.b, waterColor.a);
                } else if(value < grassThreshold) {
                    SDL_SetRenderDrawColor(renderer, swampColor.r, swampColor.g, swampColor.b, swampColor.a);
                } else {
                    SDL_SetRenderDrawColor(renderer, grassColor.r, grassColor.g, grassColor.b, grassColor.a);
                }
                
                SDL_RenderFillRect(renderer, &cell);
            }
        }
        
        // Reset render target to default
        SDL_SetRenderTarget(renderer, nullptr);
        needsUpdate = false;
        std::cout << "Texture update complete." << std::endl;
    }
    
    // Render the cached texture
    SDL_RenderCopy(renderer, terrainTexture, nullptr, nullptr);
}
