#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <memory>
#include <unordered_map>
#include <string>

class Frog {
    
    public:
        enum class State {
            IDLE,
            GRAPPLING,
            JUMPING,
            FALLING
        };

        enum class Direction {
            LEFT,
            RIGHT
        };

        // Constructor
        Frog(float startX, float startY) 
            : x(startX), y(startY), velocityX(0), velocityY(0), 
              grounded(false), currentState(State::IDLE), 
              facing(Direction::RIGHT) {
            collisionBox = {static_cast<int>(x), static_cast<int>(y), (16 * 4), (14 * 4)}; // Default size
        }

        // Destructor to clean up SDL textures
        ~Frog() {
            for (auto& pair : animations) {
                if (pair.second.spritesheet) {
                    SDL_DestroyTexture(pair.second.spritesheet);
                }
            }
        }

        // Add animation
        void addAnimation(State state, SDL_Texture* spritesheet, int frameWidth, 
                         int frameHeight, int frameCount, float frameTime) {
            Animation anim;
            anim.spritesheet = spritesheet;
            anim.frameWidth = frameWidth;
            anim.frameHeight = frameHeight;
            anim.frameCount = frameCount;
            anim.frameTime = frameTime;
            anim.currentTime = 0;
            anim.currentFrame = 0;
            animations[state] = anim;
        }

        // Update function
        void update(float deltaTime) {
            // Update physics
            if (!grounded) {
                velocityY += GRAVITY * deltaTime;
            }

            x += velocityX * deltaTime;
            y += velocityY * deltaTime;

            // Update collision box position
            collisionBox.x = static_cast<int>(x);
            collisionBox.y = static_cast<int>(y);

            // Update animation
            if (animations.count(currentState) > 0) {
                Animation& currentAnim = animations[currentState];
                currentAnim.currentTime += deltaTime;
                if (currentAnim.currentTime >= currentAnim.frameTime) {
                    currentAnim.currentTime -= currentAnim.frameTime;
                    currentAnim.currentFrame = (currentAnim.currentFrame + 1) % currentAnim.frameCount;
                }
            }
        }

        // Movement functions
        void grapple(int targetX, int targetY) {
            int distX = targetX - x, 
                distY = targetY - y;
            currentState = State::GRAPPLING;
        }

        void jump() {
            if (grounded) {
                velocityY = JUMP_FORCE;
                grounded = false;
                currentState = State::JUMPING;
            }
        }

        void stopMoving() {
            velocityX = 0;
            if (grounded) {
                currentState = State::IDLE;
            }
        }

        // Getters
        SDL_Rect getCurrentFrame() const {
            if (animations.count(currentState) > 0) {
                const Animation& currentAnim = animations.at(currentState);
                return SDL_Rect{
                    currentAnim.currentFrame * currentAnim.frameWidth,
                    0,
                    currentAnim.frameWidth,
                    currentAnim.frameHeight
                };
            }
            return SDL_Rect{0, 0, 32, 32}; // Default frame if no animation
        }

        SDL_Rect getCollisionBox() const { return collisionBox; } // gives an sdl_rect

        State getState() const { return currentState; } // gives state

        Direction getFacing() const { return facing; } // gives direction
        
        SDL_Texture* getCurrentTexture() const { // return the current texture if it exists
            return animations.count(currentState) > 0 ? 
                   animations.at(currentState).spritesheet : nullptr;
        }

        // Setters
        void setGrounded(bool isGrounded) { 
            grounded = isGrounded;
            if (grounded && currentState == State::FALLING) {
                currentState = State::IDLE;
            }
        }
    
    private:
        // Position and physics
        float x, y; // Use floats for smoother movement
        float velocityX, velocityY;
        bool grounded; // we don't want flying frogs!
        
        // Constants; apparently static constexpr is more stable than consts
        static constexpr float MOVE_SPEED = 300.0f; // fine-tune these values if you like
        static constexpr float GRAPPLE_SPEED = 700.0f;
        static constexpr float JUMP_FORCE = -500.0f;
        static constexpr float GRAVITY = 980.0f;
        
        // Current state
        State currentState;
        // Current direction (left/right)
        Direction facing;
        
        // Animation; structs help tie the variables together
        struct Animation {
            SDL_Texture* spritesheet;
            int frameWidth = 16; // width of each sprite in the spritesheet
            int frameHeight = 14; // height of each sprite in the spritesheet
            int frameCount; // number of frames in each animation
            float frameTime;  // Time per frame in seconds
            float currentTime;
            int currentFrame;
        };
        
        std::unordered_map<State, Animation> animations; // make a map that ties each animation to a state :D
        SDL_Rect collisionBox; // define a collision box
};
