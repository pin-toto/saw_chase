#ifndef COLLISION_H
#define COLLISION_H

#include <SDL2/SDL.h>

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

#endif
