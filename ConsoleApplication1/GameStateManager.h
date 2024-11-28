/*********************************************
Author: Aaron Chakine
Description: The Game State Manager class. Handles switching between states, and running current states.

Subsequent changes:
Format: [Author] - [Changes]
- 
*********************************************/


#include <stack>
#include "GameState.h"

class GameStateManager {
private:
    std::stack<GameState*> states;

public:
    // Push a new state onto the stack
    void PushState(GameState* state) {
        states.push(state);
        states.top()->Init();
    }

    // Pop the current state off the stack
    void PopState() {
        if (!states.empty()) {
            states.top()->CleanUp();
            delete states.top();
            states.pop();
        }
    }

    // Replace the current state with a new one
    void ChangeState(GameState* state) {
        if (!states.empty()) {
            states.top()->CleanUp();
            delete states.top();
            states.pop();
        }
        states.push(state);
        states.top()->Init();
    }

    // Handle events, update, and render the current state
    void HandleEvents(SDL_Event& event) {
        if (!states.empty()) {
            states.top()->HandleEvents(event);
        }
    }

    void Update(float deltaTime) {
        if (!states.empty()) {
            states.top()->Update(deltaTime);
        }
    }

    void Render(SDL_Renderer* renderer) {
        if (!states.empty()) {
            states.top()->Render(renderer);
        }
    }

    // Clean up all states
    ~GameStateManager() {
        while (!states.empty()) {
            states.top()->CleanUp();
            delete states.top();
            states.pop();
        }
    }
};
