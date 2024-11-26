#ifndef FROG_CLASS_H
#define FROG_CLASS_H

#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <cmath>

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

    // Setters
    void setGrounded(bool isGrounded);

private:
    // Position and physics
    float x, y;
    float velocityX, velocityY, fallVelocity;
    bool grounded;
    
    // Constants
    static constexpr float MOVE_SPEED = 300.0f;
    static constexpr float GRAPPLE_SPEED = 1500.0f;
    static constexpr float GRAPPLE_DRAG = 0.6f;
    static constexpr float JUMP_FORCE = -500.0f;
    static constexpr float GRAVITY = 980.0f;
    
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
