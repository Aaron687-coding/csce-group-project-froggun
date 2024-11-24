#include "frogClass.h"

// Constructor
Frog::Frog(float startX, float startY) 
    : x(startX), y(startY), velocityX(0), velocityY(0), 
      jumpHeight(0), jumpTime(0), grappleX(0), grappleY(0),
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
    const float JUMP_DURATION = 0.2f; // Total jump duration in seconds
    const float MAX_JUMP_HEIGHT = 50.0f; // Maximum height of the jump bobbing effect 

    // Update position based on velocity only during jump or grapple
    if (currentState == State::JUMPING || currentState == State::GRAPPLING) {
        x += velocityX * deltaTime;
        y += velocityY * deltaTime;

        if (currentState == State::JUMPING) {
            // Update jump time
            jumpTime += deltaTime;
            
            // Calculate jump height using a sine wave for smooth bobbing
            jumpHeight = MAX_JUMP_HEIGHT * std::sin((jumpTime / JUMP_DURATION) * M_PI);

            // Check if jump is complete
            if (jumpTime >= JUMP_DURATION) {
                jumpTime = 0;
                jumpHeight = 0;
                currentState = State::IDLE;
                velocityX = 0;
                velocityY = 0;
                grounded = true;
            }
        }
        
        // If the frog is close enough to its target during grapple, stop moving
        if (currentState == State::GRAPPLING) {
            float distX = std::abs(x - grappleX);
            float distY = std::abs(y - grappleY);
            if (distX < GRAPPLE_THRESHOLD && distY < GRAPPLE_THRESHOLD) {
                x = grappleX; // Snap to exact position
                y = grappleY;
                stopMoving();
            }
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
    
    float speedMultiplier = 1.0f;
    // If grappling during a jump, increase speed
    if (currentState == State::JUMPING) {
        speedMultiplier = 1.5f; // 50% speed boost when combo-ing with jump
    }
    
    // Normalize the direction vector and apply grapple speed
    if (distance > 0) {  // Prevent division by zero
        velocityX = (dirX / distance) * GRAPPLE_SPEED * speedMultiplier;
        velocityY = (dirY / distance) * GRAPPLE_SPEED * speedMultiplier;
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
    if (grounded || currentState == State::GRAPPLING) {
        // Get the current movement direction from input
        float dirX = velocityX != 0 ? velocityX / std::abs(velocityX) : 0;
        float dirY = velocityY != 0 ? velocityY / std::abs(velocityY) : 0;

        // If no direction is held, use facing direction for horizontal movement
        if (dirX == 0 && dirY == 0) {
            dirX = (facing == Direction::RIGHT) ? 1.0f : -1.0f;
        }

        // Normalize diagonal movement
        if (dirX != 0 && dirY != 0) {
            float normalizer = 1.0f / std::sqrt(2.0f);
            dirX *= normalizer;
            dirY *= normalizer;
        }

        velocityX = dirX * JUMP_FORCE;
        velocityY = dirY * JUMP_FORCE;

        grounded = false;
        currentState = State::JUMPING;
        jumpTime = 0;
    }
}

void Frog::move(int directionX, int directionY) {
    // Only allow movement input during jump
    if (currentState == State::JUMPING) {
        // Store the movement direction for the next jump
        if (directionX != 0 || directionY != 0) {
            // Normalize diagonal movement
            if (directionX != 0 && directionY != 0) {
                float normalizer = 1.0f / std::sqrt(2.0f);
                velocityX = directionX * MOVE_SPEED * normalizer;
                velocityY = directionY * MOVE_SPEED * normalizer;
            } else {
                velocityX = directionX * MOVE_SPEED;
                velocityY = directionY * MOVE_SPEED;
            }

            // Update facing direction based on horizontal movement
            if (directionX > 0) {
                facing = Direction::RIGHT;
            } else if (directionX < 0) {
                facing = Direction::LEFT;
            }
        }
    }
}

void Frog::stopMoving() {
    velocityX = 0;
    velocityY = 0;
    grounded = true;
    currentState = State::IDLE;
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
