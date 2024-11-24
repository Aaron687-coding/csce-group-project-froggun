#ifndef FROG_CLASS_H
#define FROG_CLASS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
    Frog(float startX, float startY);
    // Destructor
    ~Frog();

    // Add animation
    void addAnimation(State state, SDL_Texture* spritesheet, int frameWidth, 
                     int frameHeight, int frameCount, float frameTime);

    // Update function
    void update(float deltaTime);

    // Movement functions
    void grapple(int targetX, int targetY);
    void jump();
    void move(int directionX, int directionY);
    void stopMoving();

    // Getters
    SDL_Rect getCurrentFrame() const;
    SDL_Rect getCollisionBox() const;
    State getState() const;
    Direction getFacing() const;
    SDL_Texture* getCurrentTexture() const;
    bool getGrounded() const;
    float getGrappleX() const;
    float getGrappleY() const;

    // Setters
    void setGrounded(bool isGrounded);

private:
    // Position and physics
    float x, y;
    float velocityX, velocityY;
    float jumpHeight;  // Current height of jump animation
    float jumpTime;    // Current time in jump animation
    float grappleX, grappleY; // Grapple target coordinates
    bool grounded;
    
    // Constants
    static constexpr float MOVE_SPEED = 300.0f;
    static constexpr float GRAPPLE_SPEED = 1500.0f;
    static constexpr float GRAPPLE_DRAG = 0.6f;
    static constexpr float JUMP_FORCE = -500.0f;
    static constexpr float GRAPPLE_THRESHOLD = 10.0f; // Distance threshold for grapple arrival
    
    // Current state
    State currentState;
    Direction facing;
    
    // Animation
    struct Animation {
        SDL_Texture* spritesheet;
        int frameWidth = 16;
        int frameHeight = 14;
        int frameCount;
        float frameTime;
        float currentTime;
        int currentFrame;
    };
    
    std::unordered_map<State, Animation> animations;
    SDL_Rect collisionBox;
};

#endif // FROG_CLASS_H
