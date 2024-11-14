#include <SDL2/SDL.h>


class GameState {
public:
    // use virtual to generalize functions for specialized GameState classes
    virtual void Init() = 0;
    virtual void HandleEvents(SDL_Event& event) = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(SDL_Renderer* renderer) = 0;
    virtual void CleanUp() = 0;
    virtual ~GameState() {}
};
