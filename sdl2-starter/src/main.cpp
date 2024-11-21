/*********************************************
Author: Aaron Chakine
Description: The main loop, which runs game states using a state manager from the GameStateManager class. 
             See GameStateManager.h for state manager details and GameState.h for state details.

Subsequent changes:
Format: [Author] - [Changes]
- 
*********************************************/


#include <iostream>
#include <SDL2/SDL.h>
#include "GameStateManager.h"
#include "gameplay.h"

using namespace std;

// Use a State Manager to handle game states!
// NOTE: make a real state before trying to run the program

// argc and argv are used to pass arguments into the program in the terminal; this can help with debugging when
// you need to test one function
//TODO: interpret these arguments and run specific functions of the program
int main(int argc, char* argv[]) {
    // Initialize SDL and other systems
    SDL_Init(SDL_INIT_EVERYTHING);
    // Initialize our blank slate of a window
    SDL_Window* window = SDL_CreateWindow("Froggun",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_SHOWN);
    // initialize renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    GameStateManager stateManager; // Use GameStateManager class
    
    stateManager.PushState(new gameplay()); // Push a gameplay state for debugging purposes

    bool isRunning = true; // while loop variable
    SDL_Event event; // variable to get events
    Uint32 lastTime = SDL_GetTicks(); // Use to calculate delta time

    while (isRunning) {
        // Calculate delta time
        Uint32 currentTime = SDL_GetTicks(); // in milliseconds
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Divide msec by 1000 to get seconds 
        lastTime = currentTime; // Update time

        // Handle SDL events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { // If the event is the close button,
                isRunning = false;        // quit the window
            }
            stateManager.HandleEvents(event); // Otherwise, send it to the stateManager
        }

        // Update the current state
        stateManager.Update(deltaTime);

        // Render the current state
        SDL_RenderClear(renderer); // clear previous render
        stateManager.Render(renderer); // render new frame depending on current state
        SDL_RenderPresent(renderer); // present the new frame
    }

    // Clean up
    stateManager.~GameStateManager(); // Explicitly call destructor to clean up all states
    SDL_DestroyRenderer(renderer); // free the renderer from memory
    SDL_DestroyWindow(window); // free window from memory
    SDL_Quit(); // quit the sdl window

    return 0; 
}


