#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <cmath> // Added for sqrt

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
              grounded(true), currentState(State::IDLE), 
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
                fallVelocity += GRAVITY * deltaTime; // measure how fast the frog is falling
            } else
                fallVelocity = 0;

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
            // Calculate direction vector
            float dirX = targetX - x;
            float dirY = targetY - y;
            
            // Calculate slowing effects
            float drag = 1;
            
            // Calculate distance to normalize the direction
            float distance = std::sqrt(dirX * dirX + dirY * dirY);
            
            if (!grounded) {
                float yDir = velocityY / fabs(velocityY); // check the direction of velocityY
                 // prevent the frog from falling during grappling for fun reasons :D
                velocityY = (fabs(velocityY) - fallVelocity) * yDir;
                fallVelocity = 0;
            } else
                drag = GRAPPLE_DRAG; // reduce grapple speed if froggo is on the ground for mechanics reasons hehehe
            

            // Normalize the direction vector and apply grapple speed
            if (distance > 0) {  // Prevent division by zero
                velocityX = (dirX / distance) * GRAPPLE_SPEED * drag;
                velocityY = (dirY / distance) * GRAPPLE_SPEED * drag;
            }
            
            // Update facing direction based on movement
            if (velocityX > 0) {
                facing = Direction::RIGHT;
            } else if (velocityX < 0) {
                facing = Direction::LEFT;
            }
            
            
            currentState = State::GRAPPLING;
        }

        void jump() {
            if (grounded) {
                velocityY = JUMP_FORCE;
                grounded = false;
                currentState = State::JUMPING;
            }
        }

        void move (int directionX, int directionY) {
            velocityX += directionX * MOVE_SPEED;
            velocityY += directionY * MOVE_SPEED;
        }

        void stopMoving() {
            velocityX = 0;
            velocityY = 0;
            // stop the frog from falling; once it makes a full hop (its velocity turns into its negative starting velocity),
            // reset fallVelocity and velocityY, and set grounded to true
            if (fallVelocity <= -JUMP_FORCE) { // use fall velocity since velocityY also contains froggy movement
                velocityY = 0;
                fallVelocity = 0;

                grounded = true;
            }

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

        bool getGrounded() const { return grounded; } // Add getter for grounded state

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
        float velocityX, velocityY, fallVelocity;
        bool grounded; // we don't want flying frogs!
        
        // Constants; apparently static constexpr is more stable than consts
        static constexpr float MOVE_SPEED = 300.0f; // fine-tune these values if you like
        static constexpr float GRAPPLE_SPEED = 700.0f;
        static constexpr float GRAPPLE_DRAG = 0.6f; // percentage; slow down froggo if grappling on the ground by grapple_drag
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
            float currentTime; // current time; use this to animate the sprite with frameTime
            int currentFrame; // self-explanatory
        };
        
        std::unordered_map<State, Animation> animations; // make a map that ties each animation to a state :D
        SDL_Rect collisionBox; // define a collision box
};
