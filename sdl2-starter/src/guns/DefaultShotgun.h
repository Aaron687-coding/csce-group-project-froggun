#ifndef DEFAULT_SHOTGUN_H
#define DEFAULT_SHOTGUN_H

#include "GunTemplate.h"
#include <SDL2/SDL.h>
#include <random>
#include <ctime>
#include <cmath>
#include <vector>
#include <deque>

// Forward declare SDL_Texture
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Point;
struct SDL_Rect;

class DefaultShotgun : public GunTemplate {
private:
    // Textures for gun and shells
    SDL_Texture* gunTexture;      // Normal gun texture
    SDL_Texture* reloadTexture;   // Texture during reload
    SDL_Texture* shellTexture;    // Shell ejection texture
    SDL_Texture* shellIcon;       // Loaded ammo icon
    SDL_Texture* shellIconEmpty;  // Empty ammo icon
    
    // Gun position and rotation
    SDL_Point gunOffset;          // Offset from frog's center
    float gunRotation;           // Current rotation angle in degrees
    SDL_Point gunPivot;          // Pivot point for rotation
    SDL_Rect gunRect;            // Position and size of gun sprite
    
    // Shell ejection system
    struct Shell {
        SDL_Rect pos;
        float velocityX;
        float velocityY;
        float rotation;
        float lifetime;
    };
    std::vector<Shell> activeShells;

    // Particle system for bullet trails
    struct Particle {
        float x, y;
        float alpha;
        float lifetime;
    };

    struct BulletTrail {
        std::deque<Particle> particles;
        float angle;
    };

    std::map<int, BulletTrail> bulletTrails;
    std::mt19937 rng;

public:
    DefaultShotgun(SDL_Renderer* renderer);
    ~DefaultShotgun();

    void shoot(int startX, int startY, int aimX, int aimY) override;
    void setGunState(gunState state) override;
    void updateBullets() override;
    void render(SDL_Renderer* renderer, int frogX, int frogY);

private:
    void updateParticles(float deltaTime);
    void addParticlesBehindBullet(int bulletId, const bullet& b);
    void ejectShell();
    void updateGunPosition(int frogX, int frogY, int mouseX, int mouseY);
    void renderAmmoIcons(SDL_Renderer* renderer, int frogX, int frogY);
};

#endif // DEFAULT_SHOTGUN_H
