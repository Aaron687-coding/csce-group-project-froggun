#include "frogClass.h"

// Constructor
Frog::Frog(float startX, float startY) 
    : x(startX), y(startY), velocityX(0), velocityY(0), 
      grounded(true), currentState(State::IDLE), 
      facing(Direction::LEFT) {
    collisionBox = {static_cast<int>(x), static_cast<int>(y), (16 * 4), (14 * 4)}; // Default size
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
    // Update physics
    if (!grounded) {
        velocityY += GRAVITY * deltaTime;
        fallVelocity += GRAVITY * deltaTime; // measure how fast the frog is falling
    } else {
        fallVelocity = 0;
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
void Frog::grapple(int targetX, int targetY) {
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
    } else {
        drag = GRAPPLE_DRAG; // reduce grapple speed if froggo is on the ground
    }

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

void Frog::jump() {
    if (grounded) {
        velocityY = JUMP_FORCE;
        grounded = false;
        currentState = State::JUMPING;
    }
}

void Frog::move(int directionX, int directionY) {
    velocityX += directionX * MOVE_SPEED;
    velocityY += directionY * MOVE_SPEED;
}

void Frog::stopMoving() {
    velocityX = 0;
    velocityY = 0;
    if (fallVelocity <= -JUMP_FORCE) {
        velocityY = 0;
        fallVelocity = 0;
        grounded = true;
    }

    if (grounded) {
        currentState = State::IDLE;
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
    return SDL_Rect{0, 0, 32, 32}; // Default frame if no animation
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

// Setters
void Frog::setGrounded(bool isGrounded) { 
    grounded = isGrounded;
    if (grounded && currentState == State::FALLING) {
        currentState = State::IDLE;
    }
}
