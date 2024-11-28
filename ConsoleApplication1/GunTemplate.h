#ifndef GUN_TEMPLATE
#define GUN_TEMPLATE

#include <SDL.h>
#include <vector>
#include <map>
#include <memory>
#include <cmath>

// This is a general class for all sorts of guns!

class GunTemplate {
public:
    struct bullet {
        SDL_Rect bulletPos;
        int bulletSpeed;
        int bulletDamage;
        float bulletLifetime;  // in seconds; subtract from this every frame
        std::vector<int> targetPos; // x and y; find the bullet's angle to the target with this
        float angle;  // angle of bullet trajectory
    };

    enum class gunState {
        IDLE,
        AIM,
        FIRE,
        RELOAD,
        CHARGING // for railguns and the like in the future
    };

    // Constructor
    GunTemplate() : currentState(gunState::IDLE), reloadTime(1.0f), currentReloadTime(0.0f),
        fireRate(0.5f), lastShotTime(0.0f), maxAmmo(10), currentAmmo(10),
        nextBulletId(0) {}

    // Destructor
    virtual ~GunTemplate() = default;

    // Pure virtual functions that must be implemented by derived classes
    virtual void shoot() = 0;
    virtual void updateBullets() = 0;
    virtual void setGunState(gunState state) = 0;

    // Virtual functions with default implementations
    virtual void aim(int targetX, int targetY) {
        if (currentState == gunState::IDLE) {
            currentState = gunState::AIM;
            aimTargetX = targetX;
            aimTargetY = targetY;
        }
    }

    // change state to reload
    virtual void reload() {
        if (currentAmmo < maxAmmo && currentState != gunState::RELOAD) {
            currentState = gunState::RELOAD;
            currentReloadTime = reloadTime;
        }
    }


    virtual void update(float deltaTime) {
        // Update reload timer
        if (currentState == gunState::RELOAD) {
            currentReloadTime -= deltaTime;
            if (currentReloadTime <= 0) {
                currentAmmo = maxAmmo;
                currentState = gunState::IDLE;
            }
        }

        // Update shot cooldown (A.K.A. fire rate)
        if (lastShotTime > 0) {
            lastShotTime -= deltaTime;
        }

        // Update bullets
       /*for (auto it = activeBullets.begin(); it != activeBullets.end();) {
            auto& [id, bullet] = *it;
            bullet.bulletLifetime -= deltaTime;
            if (bullet.bulletLifetime <= 0) {
                it = activeBullets.erase(it);
            }
            else {
                // Update bullet position based on angle and speed
                float dx = std::cos(bullet.angle) * bullet.bulletSpeed * deltaTime;
                float dy = std::sin(bullet.angle) * bullet.bulletSpeed * deltaTime;
                bullet.bulletPos.x += static_cast<int>(dx);
                bullet.bulletPos.y += static_cast<int>(dy);
                ++it;
            }
        }*/
         
    }

    // Getters
    gunState getCurrentState() const { return currentState; }
    int getCurrentAmmo() const { return currentAmmo; }
    int getMaxAmmo() const { return maxAmmo; }
    float getReloadTime() const { return reloadTime; }
    float getCurrentReloadTime() const { return currentReloadTime; }
    const std::map<int, bullet>& getBullets() const { return activeBullets; }

    // Setters
    void setFireRate(float rate) { fireRate = rate; }
    void setMaxAmmo(int ammo) { maxAmmo = ammo; }
    void setReloadTime(float time) { reloadTime = time; }
    void setBulletSpeed(int speed) { defaultBulletSpeed = speed; }
    void setBulletDamage(int damage) { defaultBulletDamage = damage; }
    void setBulletLifetime(float lifetime) { defaultBulletLifetime = lifetime; }

protected:
    gunState currentState;
    std::map<int, bullet> activeBullets;  // map of bullet IDs to bullets
    int nextBulletId;  // counter for generating unique bullet IDs

    // Gun properties
    float reloadTime;
    float currentReloadTime;
    float fireRate;
    float lastShotTime;
    int maxAmmo;
    int currentAmmo;

    // Bullet properties
    int defaultBulletSpeed = 500;
    int defaultBulletDamage = 1;
    float defaultBulletLifetime = 2.0f;

    // Aiming properties
    int aimTargetX = 0;
    int aimTargetY = 0;

    // Helper function to create a new bullet
    void addBullet(int startX, int startY, int targetX, int targetY) {
        bullet newBullet;
        newBullet.bulletPos = { startX, startY, 8, 8 };  // Default bullet size 8x8
        newBullet.bulletSpeed = defaultBulletSpeed;
        newBullet.bulletDamage = defaultBulletDamage;
        newBullet.bulletLifetime = defaultBulletLifetime;
        newBullet.targetPos = { targetX, targetY };

        // Calculate angle between start position and target
        float dx = targetX - startX;
        float dy = targetY - startY;
        newBullet.angle = std::atan2(dy, dx);

        // Add bullet to active bullets map with a unique ID
        activeBullets[nextBulletId++] = newBullet;
    }

    // Helper function to check if can shoot
    bool canShoot() const {
        return currentState != gunState::RELOAD &&
            currentAmmo > 0 &&
            lastShotTime <= 0;
    }
};

#endif // GUN_TEMPLATE