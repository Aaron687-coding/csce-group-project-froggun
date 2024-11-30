#pragma once
#include "../GameState.h"
#include "TerrainGrid.h"
#include "../terrainElem.h"
#include "../RainSystem.h"
#include "../waterPhysics.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <iostream>

// Forward declarations
class GameStateManager;

class MenuState : public GameState {
private:
    std::shared_ptr<TerrainGrid> terrain;
    std::shared_ptr<terrainElements> terrainElems;
    std::unique_ptr<RainSystem> rainSystem;
    std::unique_ptr<WaterPhysics> waterPhysics;  // Added water physics
    bool initialized;
    GameStateManager& stateManager;
    TTF_Font* regularFont;
    TTF_Font* outlineFont;
    SDL_Color whiteColor;
    SDL_Color brownColor;

    void renderTextPair(SDL_Renderer* renderer, const char* text, int x, int y) {
        if (!outlineFont || !regularFont) {
            std::cout << "Fonts not loaded!" << std::endl;
            return;
        }

        // First render outline text to get dimensions
        SDL_Surface* outlineSurface = TTF_RenderText_Blended(outlineFont, text, brownColor);
        if (!outlineSurface) {
            std::cout << "Failed to render outline text surface: " << TTF_GetError() << std::endl;
            return;
        }

        // Create texture for outline text
        SDL_Texture* outlineTexture = SDL_CreateTextureFromSurface(renderer, outlineSurface);
        if (!outlineTexture) {
            std::cout << "Failed to create outline texture: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(outlineSurface);
            return;
        }

        // Get dimensions from outline surface
        int width = outlineSurface->w;
        int height = outlineSurface->h;

        // Now render regular text
        SDL_Surface* regularSurface = TTF_RenderText_Blended(regularFont, text, whiteColor);
        if (!regularSurface) {
            std::cout << "Failed to render regular text surface: " << TTF_GetError() << std::endl;
            SDL_FreeSurface(outlineSurface);
            SDL_DestroyTexture(outlineTexture);
            return;
        }

        SDL_Texture* regularTexture = SDL_CreateTextureFromSurface(renderer, regularSurface);
        if (!regularTexture) {
            std::cout << "Failed to create regular texture: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(outlineSurface);
            SDL_FreeSurface(regularSurface);
            SDL_DestroyTexture(outlineTexture);
            return;
        }

        // Set up destination rectangles using outline dimensions for both
        SDL_Rect destRect = {x, y, width, height};

        // Render outline text first (brown)
        SDL_RenderCopy(renderer, outlineTexture, NULL, &destRect);

        // Render regular text (white) on top with a slight offset
        SDL_Rect regularRect = {x, y + 1, width - 1, height};
        SDL_RenderCopy(renderer, regularTexture, NULL, &regularRect);

        // Clean up
        SDL_FreeSurface(outlineSurface);
        SDL_FreeSurface(regularSurface);
        SDL_DestroyTexture(outlineTexture);
        SDL_DestroyTexture(regularTexture);
    }

    TTF_Font* loadFont(const char* filename, int size) {
        // Try build directory first
        TTF_Font* font = TTF_OpenFont((std::string("build/debug/fonts/") + filename).c_str(), size);
        if (!font) {
            // Try root fonts directory
            font = TTF_OpenFont((std::string("fonts/") + filename).c_str(), size);
            if (!font) {
                std::cout << "Failed to load font " << filename << ": " << TTF_GetError() << std::endl;
                std::cout << "Tried paths: build/debug/fonts/" << filename << " and fonts/" << filename << std::endl;
            }
        }
        return font;
    }

public:
    MenuState(GameStateManager& manager) 
        : initialized(false), stateManager(manager), regularFont(nullptr), outlineFont(nullptr) {
        whiteColor = {255, 255, 255, 255}; // White
        brownColor = {154, 77, 1, 255};    // Brown
        rainSystem = std::make_unique<RainSystem>(1280, 720); // Initialize rain system with screen dimensions
    }

    void Init() override {
        std::cout << "MenuState initialized" << std::endl;
        initialized = false;

        // Load both fonts with the same size
        regularFont = loadFont("pixelFont.ttf", 16);
        outlineFont = loadFont("pixelFontOutline.ttf", 16);
    }

    void Update(float deltaTime) override {
        if (rainSystem) {
            rainSystem->update(deltaTime);
        }
        
        // Update water physics
        if (waterPhysics && terrain) {
            waterPhysics->update(deltaTime, *terrain);
        }
    }

    void HandleEvents(SDL_Event& event) override;  // Definition moved to cpp file

    void Render(SDL_Renderer* renderer) override {
        if (!initialized || !terrain) {
            std::cout << "Creating terrain..." << std::endl;
             // Use 1280, 720, 1 for smoother maps :)
            terrain = std::make_shared<TerrainGrid>(renderer, 64, 36, 20);
            terrain->generate();
            terrainElems = std::make_shared<terrainElements>(renderer, terrain.get(), 1280, 720);
            terrainElems->generate();
            waterPhysics = std::make_unique<WaterPhysics>(renderer);  // Initialize water physics
            initialized = true;
            std::cout << "Terrain created" << std::endl;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        terrain->render(renderer);
        terrainElems->render();
        
        // Render water effects after terrain but before rain
        if (waterPhysics) {
            waterPhysics->render(renderer);
        }
        
        // Render rain after water effects but before UI
        if (rainSystem) {
            rainSystem->render(renderer);
        }
        
        // Render text instructions
        renderTextPair(renderer, "PRESS W/S TO ADJUST WATER LEVEL", 100, 100);
        renderTextPair(renderer, "PRESS E/D TO ADJUST TERRAIN LEVEL", 100, 150);
        renderTextPair(renderer, "PRESS R TO REGENERATE MAP", 100, 200);
        renderTextPair(renderer, "PRESS ENTER OR SPACE TO START GAME", 100, 250);
    }

    void CleanUp() override {
        std::cout << "MenuState cleanup" << std::endl;
        if (regularFont) {
            TTF_CloseFont(regularFont);
            regularFont = nullptr;
        }
        if (outlineFont) {
            TTF_CloseFont(outlineFont);
            outlineFont = nullptr;
        }
    }

    // Getter methods for terrain
    std::shared_ptr<TerrainGrid> getTerrain() const { return terrain; }
    std::shared_ptr<terrainElements> getTerrainElements() const { return terrainElems; }
};
