#include "computer.h"

void moveTroops(map::city* choose, map::city* goTo, map::world* world) {
    unsigned int amountOfTroops = 5;
    choose->amount -= amountOfTroops;
    map::troops newTroops(amountOfTroops, choose->team, choose->pos);
    newTroops.goTo = goTo;
    world->troops.insert(world->troops.begin(), newTroops);
}

void ai::computer::updateComputer (map::world* world) {
    for (int i = 0; i < map::numOfCities; i++) {
        if (world->cities[i].team != team)
            continue;
        for (int b = 0; b < world->cities[i].bridges.size(); b++) {
            if (world->cities[i].bridges[b]->team == team)
                continue;
            if (world->cities[i].bridges[b]->amount < world->cities[i].amount)
                moveTroops(&world->cities[i], world->cities[i].bridges[b], world);
        }
    }        
}