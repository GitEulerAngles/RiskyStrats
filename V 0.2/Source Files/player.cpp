#include "SDL.h"
#include "player.h"

void ply::player::getPlayerInput(bool* isGameRunning) {
    SDL_Event e;
    SDL_PumpEvents();
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_WINDOWEVENT) {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                *isGameRunning = false;
                break;
            }
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            switch (e.button.button) {
            case SDL_BUTTON_MIDDLE:
                middle = true;
                break;
            case SDL_BUTTON_LEFT:
                left = true;
                break;
            case SDL_BUTTON_RIGHT:
                right = true;
                break;
            }
        }
        if (e.type == SDL_MOUSEBUTTONUP) {
            switch (e.button.button) {
            case SDL_BUTTON_MIDDLE:
                middle = false;
                break;
            case SDL_BUTTON_LEFT:
                left = false;
                break;
            case SDL_BUTTON_RIGHT:
                right = false;
                break;
            }
        }
    }
}