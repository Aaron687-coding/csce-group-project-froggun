/*********************************************
Author: Aaron Chakine
Description: The main loop, which runs game states using a state manager from the GameStateManager class. 
             See GameStateManager.h for state manager details and GameState.h for state details.

Subsequent changes:
Format: [Author] - [Changes]
- 
*********************************************/

// SCREENSIZE IS 1280x720

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "GameStateManager.h"
#include "gameplay.h"

using namespace std;

int main(int argc, char* argv[]) {
    // Initialize SDL and other systems
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return 1;
    }

    // Initialize SDL_image with PNG support
    int imgFlags = IMG_INIT_PNG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // Initialize our blank slate of a window
    SDL_Window* window = SDL_CreateWindow("Froggun",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        1280, 720, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // initialize renderer with hardware acceleration and vsync
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    try {
        GameStateManager stateManager;
        stateManager.PushState(new gameplay());

        bool isRunning = true;
        SDL_Event event;
        Uint32 lastTime = SDL_GetTicks();

        while (isRunning) {
            // Calculate delta time
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;

            // Handle SDL events
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    isRunning = false;
                }
                stateManager.HandleEvents(event);
            }

            // Update and render
            stateManager.Update(deltaTime);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            stateManager.Render(renderer);
            SDL_RenderPresent(renderer);
        }
    }
    catch (const std::exception& e) {
        cout << "Error occurred: " << e.what() << endl;
    }

    // Clean up in reverse order of creation
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
