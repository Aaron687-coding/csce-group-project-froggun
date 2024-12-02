#ifndef FROG_CLASS_H
#define FROG_CLASS_H

#include <SDL.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <cmath>
#include "../healthBar.cpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Forward declare SDL_image functions we need
extern "C" {
    SDL_Texture* IMG_LoadTexture(SDL_Renderer* renderer, const char* file);
    const char* IMG_GetError(void);
}

class Frog {
public:
    enum class State {
        IDLE,
        GRAPPLING,
        JUMPING,
        FALLING,
        DEAD
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
    void jump(float directionX, float directionY);
    void stopMoving();

    // Health functions
    void initializeHealthBar(SDL_Renderer* renderer, int maxHealth = 100);
    void takeDamage(int amount);
    bool isAlive() const { return health > 0; }
    void drawHealthBar();

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
    void setOnWater(bool isOnWater) { onWater = isOnWater; }
    void setVXZero(bool positive) {
        if (positive && velocityX > 0 || !positive && velocityX < 0) { velocityX = 0; } 
    }
    void setVYZero(bool positive) {
        if (positive && velocityY > 0 || !positive && velocityY < 0) { velocityY = 0; } 
    }

private:
    // Position and physics
    float x, y;
    float velocityX, velocityY;
    float jumpHeight;  // Current height of jump animation
    float jumpTime;    // Current time in jump animation
    float grappleX, grappleY; // Grapple target coordinates
    bool grounded;
    bool onWater;
    
    // Health system
    std::unique_ptr<healthBar> hpBar;
    int health;
    
    // Constants
    static constexpr float BASE_MOVE_SPEED = 300.0f;
    static constexpr float BASE_GRAPPLE_SPEED = 500.0f;
    static constexpr float WATER_SPEED_MULTIPLIER = 0.5f;
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

    // Helper methods to get current speeds based on terrain
    float getCurrentMoveSpeed() const {
        return onWater ? BASE_MOVE_SPEED * WATER_SPEED_MULTIPLIER : BASE_MOVE_SPEED;
    }
    
    float getCurrentGrappleSpeed() const {
        return onWater ? BASE_GRAPPLE_SPEED * WATER_SPEED_MULTIPLIER : BASE_GRAPPLE_SPEED;
    }
};

#endif // FROG_CLASS_H
