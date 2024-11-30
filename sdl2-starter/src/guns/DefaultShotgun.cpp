#include "DefaultShotgun.h"
#include <SDL2/SDL_image.h>

DefaultShotgun::DefaultShotgun(SDL_Renderer* renderer) : GunTemplate() {
    // Initialize gun properties with lower ammo count
    setMaxAmmo(2);  // Only 2 shells at a time
    setReloadTime(1.0f);
    setFireRate(0.2f);
    setBulletSpeed(800);
    setBulletDamage(3);
    setBulletLifetime(0.3f);
    
    // Load textures
    gunTexture = IMG_LoadTexture(renderer, "assets/shotgun.png");
    reloadTexture = IMG_LoadTexture(renderer, "assets/shotgunReload.png");
    shellTexture = IMG_LoadTexture(renderer, "assets/medShell.png");
    shellIcon = IMG_LoadTexture(renderer, "assets/shellIcon.png");
    shellIconEmpty = IMG_LoadTexture(renderer, "assets/noShellIcon.png");
    
    if (!gunTexture || !reloadTexture || !shellTexture) {
        SDL_Log("Failed to load shotgun textures: %s", IMG_GetError());
    }
    
    // Initialize gun position and size
    int width = 39 * 1.5;   // actual size * scale
    int height = 22 * 1.5;   // actual size * scale
    gunRect = {0, 0, width, height};  // Adjust size based on your sprite
    gunOffset = {0, 12};       // Offset from frog's center
    gunPivot = {10, 10};       // Pivot point for rotation (adjust based on sprite)
    gunRotation = 0.0f;
    
    // Initialize random number generator
    rng.seed(std::time(nullptr));
}

DefaultShotgun::~DefaultShotgun() {
    // Clean up textures
    if (gunTexture) SDL_DestroyTexture(gunTexture);
    if (reloadTexture) SDL_DestroyTexture(reloadTexture);
    if (shellTexture) SDL_DestroyTexture(shellTexture);
    if (shellIcon) SDL_DestroyTexture(shellIcon);
    if (shellIconEmpty) SDL_DestroyTexture(shellIconEmpty);
}

void DefaultShotgun::shoot(int startX, int startY, int aimX, int aimY) {
    if (!canShoot()) return;

    // Random distribution for spread (-22.5 to +22.5 degrees)
    std::uniform_real_distribution<float> spreadDist(-M_PI/8, M_PI/8);
    
    // Base angle from gun to target
    float baseAngle = std::atan2(aimY - startY, aimX - startX);

    // Fire 8 pellets
    for (int i = 0; i < 8; i++) {
        float spreadAngle = baseAngle + spreadDist(rng);
        
        // Calculate spread target position
        float targetDist = 100.0f;  // arbitrary distance
        int spreadTargetX = startX + static_cast<int>(cos(spreadAngle) * targetDist);
        int spreadTargetY = startY + static_cast<int>(sin(spreadAngle) * targetDist);
        
        // Add bullet and initialize its trail
        addBullet(startX, startY, spreadTargetX, spreadTargetY);
        bulletTrails[nextBulletId-1] = BulletTrail();
        bulletTrails[nextBulletId-1].angle = spreadAngle;
    }

    // Decrease ammo and set cooldown
    currentAmmo--;
    lastShotTime = fireRate;
}

void DefaultShotgun::setGunState(gunState state) {
    if (state == gunState::RELOAD && currentState != gunState::RELOAD) {
        // Eject shells when starting reload
        ejectShell();
        ejectShell();
    }
    currentState = state;
}

void DefaultShotgun::updateBullets() {
    // Update bullet trails
    for (auto& [id, trail] : bulletTrails) {
        // Add new particles behind the bullet
        if (activeBullets.count(id) > 0) {
            addParticlesBehindBullet(id, activeBullets[id]);
        }
        
        // Update existing particles
        auto& particles = trail.particles;
        for (auto it = particles.begin(); it != particles.end();) {
            it->lifetime -= 0.016f;  // Assuming 60 FPS
            it->alpha = (it->lifetime / 0.2f) * 255;  // Fade out over 0.2 seconds
            
            if (it->lifetime <= 0) {
                it = particles.erase(it);
            } else {
                ++it;
            }
        }
    }

    // Clean up trails for deleted bullets
    for (auto it = bulletTrails.begin(); it != bulletTrails.end();) {
        if (activeBullets.count(it->first) == 0 && it->second.particles.empty()) {
            it = bulletTrails.erase(it);
        } else {
            ++it;
        }
    }

    // Update shells
    for (auto it = activeShells.begin(); it != activeShells.end();) {
        it->velocityY += 500.0f * 0.016f;  // Gravity
        it->pos.x += static_cast<int>(it->velocityX * 0.016f);
        it->pos.y += static_cast<int>(it->velocityY * 0.016f);
        it->rotation += 360.0f * 0.016f;  // Rotate 360 degrees per second
        it->lifetime -= 0.016f;

        if (it->lifetime <= 0) {
            it = activeShells.erase(it);
        } else {
            ++it;
        }
    }
}

void DefaultShotgun::updateGunPosition(int frogX, int frogY, int mouseX, int mouseY) {
    // Calculate angle between frog and mouse
    float dx = mouseX - frogX;
    float dy = mouseY - frogY;
    gunRotation = (atan2(dy, dx) * 180.0f / M_PI);
    
    // Update gun position relative to frog
    gunRect.x = frogX - gunPivot.x + gunOffset.x;  // Added gunOffset
    gunRect.y = frogY - gunPivot.y + gunOffset.y;  // Added gunOffset
}

void DefaultShotgun::renderAmmoIcons(SDL_Renderer* renderer, int frogX, int frogY) {
    const int ICON_SIZE = 32;  // Size of ammo icons
    const int ICON_SPACING = 25;  // Space between icons
    const int ICON_OFFSET_Y = 40;  // Distance below frog
    
    // Calculate starting position for first icon
    int startX = frogX - ((maxAmmo * ICON_SPACING) / 2);
    int y = frogY + ICON_OFFSET_Y;
    
    // Render icons for each ammo slot
    for (int i = 0; i < maxAmmo; i++) {
        SDL_Rect iconRect = {
            startX + (i * ICON_SPACING),
            y,
            static_cast<int>(ICON_SIZE * (5 / 8.0f)), // multiply so that the pixels are not bent
            ICON_SIZE 
        };
        
        // Choose texture based on whether this slot has ammo
        SDL_Texture* iconTexture = (i < currentAmmo) ? shellIcon : shellIconEmpty;
        SDL_RenderCopy(renderer, iconTexture, nullptr, &iconRect);
    }
}

void DefaultShotgun::render(SDL_Renderer* renderer, int frogX, int frogY) {
    // Enable alpha blending
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Get mouse position for aiming
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    updateGunPosition(frogX, frogY, mouseX, mouseY);

    // Render bullet trails
    for (const auto& [id, trail] : bulletTrails) {
        for (const auto& particle : trail.particles) {
            // Set color to bright yellow with alpha
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, static_cast<Uint8>(particle.alpha));
            
            // Draw particle as a small rectangle
            SDL_Rect particleRect = {
                static_cast<int>(particle.x - 2),
                static_cast<int>(particle.y - 2),
                4, 4
            };
            SDL_RenderFillRect(renderer, &particleRect);
        }

        // If bullet is still active, draw a bright streak at its position
        if (activeBullets.count(id) > 0) {
            const auto& bullet = activeBullets[id];
            
            // Draw a bright yellow streak in the direction of movement
            float streakLength = 12.0f;
            int endX = bullet.bulletPos.x - static_cast<int>(cos(trail.angle) * streakLength);
            int endY = bullet.bulletPos.y - static_cast<int>(sin(trail.angle) * streakLength);
            
            // Draw multiple lines for thickness
            for (int i = -1; i <= 1; i++) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderDrawLine(renderer,
                    bullet.bulletPos.x + i, bullet.bulletPos.y,
                    endX + i, endY);
            }
        }
    }

    // Render shells
    for (const auto& shell : activeShells) {
        SDL_Rect destRect = shell.pos;
        SDL_Point center = {destRect.w / 2, destRect.h / 2};
        SDL_RenderCopyEx(renderer, shellTexture, nullptr, &destRect, 
                       shell.rotation, &center, SDL_FLIP_NONE);
    }

    // Render gun sprite based on state
    SDL_Texture* currentTexture = (currentState == gunState::RELOAD) ? reloadTexture : gunTexture;
    if (currentTexture) {
        // Determine if gun should be flipped based on mouse position
        SDL_RendererFlip flip = (mouseX < frogX) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
        
        SDL_RenderCopyEx(renderer, currentTexture, nullptr, &gunRect, 
                        gunRotation, &gunPivot, flip);
    }

    // Render ammo icons
    renderAmmoIcons(renderer, frogX, frogY);
}

void DefaultShotgun::addParticlesBehindBullet(int bulletId, const bullet& b) {
    auto& trail = bulletTrails[bulletId];
    
    // Add new particle at bullet's position
    Particle p;
    p.x = b.bulletPos.x;
    p.y = b.bulletPos.y;
    p.lifetime = 0.2f;  // Particle lives for 0.2 seconds
    p.alpha = 255;
    
    trail.particles.push_front(p);
    
    // Limit number of particles per trail
    if (trail.particles.size() > 10) {
        trail.particles.pop_back();
    }
}

void DefaultShotgun::ejectShell() {
    Shell shell;
    shell.pos = {gunRect.x + gunPivot.x, gunRect.y + gunPivot.y, 16, 8};  // Eject from gun position
    
    // Random velocities for natural movement
    std::uniform_real_distribution<float> velDist(-100.0f, 100.0f);
    shell.velocityX = velDist(rng);
    shell.velocityY = -200.0f;  // Initial upward velocity
    
    shell.rotation = 0.0f;
    shell.lifetime = 2.0f;  // Shell disappears after 2 seconds
    
    activeShells.push_back(shell);
}
