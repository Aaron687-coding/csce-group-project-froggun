#include "MenuState.h"
#include "../gameplay.h"
#include "../GameStateManager.h"

void MenuState::HandleEvents(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        std::cout << "Key pressed: " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
        
        // Ensure terrain is initialized before handling any terrain-related keys
        if (!initialized || !terrain) {
            std::cout << "Terrain not initialized, initializing now..." << std::endl;
            return;
        }

        switch (event.key.keysym.sym) {
            case SDLK_RETURN:
            case SDLK_SPACE: {
                std::cout << "Starting game..." << std::endl;
                gameplay* gameplayState = new gameplay(stateManager);
                gameplayState->setTerrain(terrain);
                gameplayState->setTerrainElements(terrainElems);
                stateManager.PushState(gameplayState);  // Use PushState instead of ChangeState
                return;
            }
            case SDLK_w:
                std::cout << "Adjusting water threshold up" << std::endl;
                terrain->setWaterThreshold(terrain->getWaterThreshold() + 0.05f);
                break;
            case SDLK_s:
                std::cout << "Adjusting water threshold down" << std::endl;
                terrain->setWaterThreshold(terrain->getWaterThreshold() - 0.05f);
                break;
            case SDLK_e:
                std::cout << "Adjusting grass threshold up" << std::endl;
                terrain->setGrassThreshold(terrain->getGrassThreshold() + 0.05f);
                break;
            case SDLK_d:
                std::cout << "Adjusting grass threshold down" << std::endl;
                terrain->setGrassThreshold(terrain->getGrassThreshold() - 0.05f);
                break;
            case SDLK_r:
                std::cout << "Regenerating terrain..." << std::endl;
                terrain->generate();
                if (terrainElems) {
                    terrainElems->generate();
                }
                break;
            default:
                std::cout << "Unhandled key press" << std::endl;
                break;
        }
    }
}
