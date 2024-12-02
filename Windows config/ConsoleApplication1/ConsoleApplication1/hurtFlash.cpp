#include "hurtFlash.h"

hurtFlash* hurtFlash::instance = nullptr;

hurtFlash* hurtFlash::getInstance() {
    if (instance == nullptr) {
        instance = new hurtFlash();
    }
    return instance;
}

void hurtFlash::update(float deltaTime) {
    // Update all flashing objects
    auto it = flashingObjects.begin();
    while (it != flashingObjects.end()) {
        it->second -= deltaTime;
        if (it->second <= 0) {
            it = flashingObjects.erase(it);
        } else {
            ++it;
        }
    }
}

SDL_Texture* hurtFlash::getFilledImage(SDL_Renderer* renderer, SDL_Texture* tex, void* objectPtr) {
    // Check if it is currently flashing
    auto it = flashingObjects.find(objectPtr);
    if (it == flashingObjects.end() || it->second <= 0) {
        return tex;
    }

    // Get texture dimensions and format
    int w, h;
    Uint32 format;
    SDL_QueryTexture(tex, &format, nullptr, &w, &h);

    // Create a surface from the texture
    // RGBA32 format allows for consistent pixel manipulation
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        return tex;
    }

    // Set the render target to a temporary texture to read pixel data
    SDL_Texture* tempTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, 
                                           SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetRenderTarget(renderer, tempTex);
    SDL_RenderCopy(renderer, tex, nullptr, nullptr);
    
    // Read pixels from the temporary texture
    SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA32, 
                        surface->pixels, surface->pitch);
    
    // Reset render target
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_DestroyTexture(tempTex);

    // Calculate red tint intensity
    Uint8 redIntensity = static_cast<Uint8>((it->second / flashTime) * 255);

    // Modify only opaque pixels
    Uint32* pixels = static_cast<Uint32*>(surface->pixels);
    int totalPixels = w * h;
    
    // process each pixel
    for (int i = 0; i < totalPixels; i++) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], surface->format, &r, &g, &b, &a);
        
        if (a > 0) {  // Only modify pixels that aren't fully transparent
            // Add red tint while preserving original color and alpha
            r = static_cast<Uint8>(std::min(255, r + redIntensity));
            pixels[i] = SDL_MapRGBA(surface->format, r, g, b, a);
        }
    }

    // Create new texture from modified surface
    SDL_Texture* redTinted = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return redTinted;
}

void hurtFlash::startFlash(void* objectPtr) {
    flashingObjects[objectPtr] = flashTime;
}
