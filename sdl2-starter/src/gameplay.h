#include "GameState.h"
#include "frogClass.cpp"

class gameplay : public GameState {
public:
    // Call override for clarity (and easier error-checking; 
    // if you spelled the function wrong it would immediately display an error)
    void Init() override {
        enum class Direction {
            NONE,
            UP, 
            DOWN,
            LEFT,
            RIGHT,
        };

        Frog frog;
    };
    void HandleEvents(SDL_Event& event) override {
        switch(event.type) {
            Uint8 const *keys = SDL_GetKeyboardState(nullptr);

            case SDL_KEYDOWN:
                if (keys[SDL_SCANCODE_W])
                    
        }
    };
    void Update(float deltaTime) override {};
    void Render(SDL_Renderer* renderer) override {};
    void CleanUp() override {};
};