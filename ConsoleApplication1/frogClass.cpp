#include "frogClass.h"

// Constructor
Frog::Frog(float startX, float startY)
    : x(startX), y(startY), velocityX(0), velocityY(0),
    jumpHeight(0), jumpTime(0), grappleX(0), grappleY(0),
    grounded(true), currentState(State::IDLE),
    facing(Direction::LEFT) {
    collisionBox = { static_cast<int>(x), static_cast<int>(y), (16 * 4), (14 * 4) }; // Default size
}

// Destructor
Frog::~Frog() {
    for (auto& pair : animations) {
        if (pair.second.spritesheet) {
            SDL_DestroyTexture(pair.second.spritesheet);
        }
    }
}

// Add animation
void Frog::addAnimation(State state, SDL_Texture* spritesheet, int frameWidth,
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
void Frog::update(float deltaTime) {
    const float JUMP_DURATION = 0.2f; // Total jump duration in seconds
    const float MAX_JUMP_HEIGHT = 80.0f; // Maximum height of the jump bobbing effect 

    // Update position based on velocity
    x += velocityX * deltaTime;
    y += velocityY * deltaTime;

    // Handle jump physics
    if (currentState == State::JUMPING) {
        // Update jump time
        jumpTime += deltaTime;

        // Calculate jump height using a sine wave for smooth bobbing
        jumpHeight = MAX_JUMP_HEIGHT * std::sin((jumpTime / JUMP_DURATION) * M_PI);

        // Only reset jump if we're not grappling
        if (jumpTime >= JUMP_DURATION && currentState != State::GRAPPLING) {
            jumpTime = 0;
            jumpHeight = 0;
            currentState = State::IDLE;
            velocityX = 0;
            velocityY = 0;
            grounded = true;
        }
    }

    // Handle grapple physics
    if (currentState == State::GRAPPLING) {
        float distX = std::abs(x - grappleX);
        float distY = std::abs(y - grappleY);

        // Calculate current direction to target
        float dirX = grappleX - x;
        float dirY = grappleY - y;
        float distance = std::sqrt(dirX * dirX + dirY * dirY);

        // Update velocities to maintain course to target
        if (distance > 0) {
            float speedMultiplier = (jumpTime > 0) ? 2.5f : 1.0f;
            velocityX = (dirX / distance) * GRAPPLE_SPEED * speedMultiplier;
            velocityY = (dirY / distance) * GRAPPLE_SPEED * speedMultiplier;
        }

        // Check if we've reached the target
        if (distX < GRAPPLE_THRESHOLD && distY < GRAPPLE_THRESHOLD) {
            x = grappleX; // Snap to exact position
            y = grappleY;
            stopMoving();
        }
    }

    // Update collision box position
    collisionBox.x = static_cast<int>(x);
    collisionBox.y = static_cast<int>(y - jumpHeight); // Apply jump height to visual position

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
void Frog::grapple(int targetX, int targetY) {
    // Calculate direction vector
    float dirX = targetX - x;
    float dirY = targetY - y;

    // Store target values
    grappleX = targetX;
    grappleY = targetY;

    // Calculate distance to normalize the direction
    float distance = std::sqrt(dirX * dirX + dirY * dirY);

    // Set initial velocities based on direction
    if (distance > 0) {  // Prevent division by zero
        float speedMultiplier = (currentState == State::JUMPING) ? 2.5f : 1.0f;
        velocityX = (dirX / distance) * GRAPPLE_SPEED * speedMultiplier;
        velocityY = (dirY / distance) * GRAPPLE_SPEED * speedMultiplier;
    }

    // Update facing direction based on movement
    if (velocityX > 0) {
        facing = Direction::RIGHT;
    }
    else if (velocityX < 0) {
        facing = Direction::LEFT;
    }

    // Maintain jump state if we're jumping
    if (currentState != State::JUMPING) {
        jumpTime = 0;
        jumpHeight = 0;
    }

    currentState = State::GRAPPLING;
}

void Frog::jump(float directionX, float directionY) {
    if (grounded || currentState == State::GRAPPLING) {
        // Update facing direction based on horizontal movement
        if (directionX > 0) {
            facing = Direction::LEFT;
        }
        else if (directionX < 0) {
            facing = Direction::RIGHT;
        }

        // Normalize diagonal movement
        if (directionX != 0 && directionY != 0) {
            float normalizer = 1.0f / std::sqrt(2.0f);
            directionX *= normalizer;
            directionY *= normalizer;
        }

        velocityX = directionX * JUMP_FORCE + directionX * MOVE_SPEED;
        velocityY = directionY * JUMP_FORCE + directionY * MOVE_SPEED;

        grounded = false;
        currentState = State::JUMPING;
        jumpTime = 0;
    }
}

void Frog::stopMoving() {
    // Stop moving if not jumping
    if (currentState != State::FALLING && currentState != State::JUMPING) {
        velocityX = 0;
        velocityY = 0;
        grounded = true;
        currentState = State::IDLE;
        jumpTime = 0;
        jumpHeight = 0;
    }
}

// Getters
SDL_Rect Frog::getCurrentFrame() const {
    if (animations.count(currentState) > 0) {
        const Animation& currentAnim = animations.at(currentState);
        return SDL_Rect{
            currentAnim.currentFrame * currentAnim.frameWidth,
            0,
            currentAnim.frameWidth,
            currentAnim.frameHeight
        };
    }
    return SDL_Rect{ 0, 0, 32, 32 }; // Default frame if no animation
}

SDL_Rect Frog::getCollisionBox() const {
    return collisionBox;
}

Frog::State Frog::getState() const {
    return currentState;
}

Frog::Direction Frog::getFacing() const {
    return facing;
}

SDL_Texture* Frog::getCurrentTexture() const {
    return animations.count(currentState) > 0 ?
        animations.at(currentState).spritesheet : nullptr;
}

bool Frog::getGrounded() const {
    return grounded;
}

float Frog::getGrappleX() const {
    return grappleX;
}

float Frog::getGrappleY() const {
    return grappleY;
}

// Setters
void Frog::setGrounded(bool isGrounded) {
    grounded = isGrounded;
    if (grounded && currentState == State::JUMPING) {
        currentState = State::IDLE;
        velocityX = 0;
        velocityY = 0;
        jumpHeight = 0;
        jumpTime = 0;
    }
}