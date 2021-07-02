#include "window.hpp"
#include "map.h"

map::world* world = new map::world;

basic::Vector2i mousePos;
bool left;
bool right;
bool middle;

void window::setup() {
    map::MAPinit();

    map::generation(width, height, world);
}

void window::input() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_WINDOWEVENT) {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                running = false;
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

void window::update() {
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
}

void window::render() {
    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i < map::numOfCities; i++) {
        world->cities[i].drawCity(renderer);
        world->cities[i].drawBridges(renderer);
        //std::cout << world->cities[i].pos.x << ' ' << world->cities[i].pos.y << std::endl;
    }

    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(screen);
}

void window::clean() {
    SDL_Quit();
}
