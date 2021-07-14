#include "window.hpp"
#include "map.h"
#include "player.h"
#include "computer.h"

map::world* world = new map::world;
ply::player* player = new ply::player;
ai::computer* com = new ai::computer;

void window::setup() {
    TTF_Init();
    map::generation(width, height-100, world);
    map::MAPinit(&world->troopsText);
}

void window::input() {
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    player->getPlayerInput(&running);
    world->getSelectedInput(keystates, player);
    world->getTroopsInput(keystates, player);
}

void window::update() {
    for (int i = 0; i < player->selected_cityID.size(); i++)
        world->setTroopsInput(player, &world->cities[player->selected_cityID[i]]);
    map::updateTroops(world);
    map::updateCities(world);
    com->updateComputer(world);
}

void window::render() {
    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < map::numOfCities; i++)
        world->cities[i].drawBridges(renderer);
    for (int i = 0; i < map::numOfCities; i++) 
        world->cities[i].drawCity(renderer, player, &world->troopsText);
    for (int i = 0; i < world->troops.size(); i++)
        world->troops[i].draw(renderer);

    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(screen);
}

void window::clean() {
    SDL_Quit();
}
