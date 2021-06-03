#include "map.h"
#include <iostream>

basic::Color Colors[9] ={
    {240,128,128},
    {0,255,0},
    {173,255,47},
    {102,205,170},
    {147,112,219},
    {255,0,0},
    {205,133,63},
    {176,196,222},
    {0,0,255}
};

int drawCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

int fillCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {

        status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
            x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
            x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
            x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
            x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

void normalizePosition(int width, int height, basic::Vector2f* p) {
    if (p->x > width-map::borderOffset)
        p->x = width-map::borderOffset;
    else if (p->x < map::borderOffset)
        p->x = map::borderOffset;
    if (p->y > height-map::borderOffset)
        p->y = height-map::borderOffset;
    else if (p->y < map::borderOffset)
        p->y = map::borderOffset;
}

bool recursionCheck(int width, int height, map::world* world) {
    bool collisionCheck = true;

    for (int c_1 = 0; c_1 < map::numOfCities; c_1++) {
        map::city* city_1 = &world->cities[c_1];
        normalizePosition(width, height, &city_1->pos);
        for (int c_2 = 0; c_2 < map::numOfCities; c_2++) {
            if (c_1 == c_2)
                continue;
            map::city* city_2 = &world->cities[c_2];
            if (round(city_1->pos.x) == round(city_2->pos.x) && round(city_1->pos.y) == round(city_2->pos.y)) {
                collisionCheck = false;
                city_2->pos.x = rand()%width;
                city_2->pos.y = rand()%height;
            }

            float dist = basic::distancef(city_1->pos, city_2->pos);

            if (dist < map::distanceBetweenCities) {
                collisionCheck = false;
                float rad = atan2f(city_1->pos.y - city_2->pos.y, city_1->pos.x - city_2->pos.x);

                city_2->pos.x += cos(rad)*dist;
                city_2->pos.y += sin(rad)*dist;
            }
        }
    }

    return collisionCheck;
}

bool InVector(map::city* city, std::vector <map::city*> cities) {
    for (int i = 0; i < cities.size(); i++)
        if (city == cities[i])
            return true;
    return false;
}

std::vector <map::city*> setColor(int ID, map::world* world) {
    std::vector <map::city*> open, closed;
    open.insert(open.begin(), world->cities[ID].bridges.begin(), world->cities[ID].bridges.end());
    closed.push_back(&world->cities[ID]);

    while (open.size() != 0) {
        int deleteTo = open.size();

        for (int i = 0; i < deleteTo; i++)
        for (int y = 0; y < open[i]->bridges.size(); y++)
            if (!InVector(open[i]->bridges[y], closed))
                open.push_back(open[i]->bridges[y]);

        closed.insert(closed.begin(), open.begin(), open.begin()+deleteTo);
        open.erase(open.begin(), open.begin()+deleteTo);
    }

    return closed;
}

std::pair<map::city*, map::city*> getClosestnewLand(std::vector <map::city*> land1, std::vector <map::city*> land2) {
    map::city* city1;
    map::city* city2;
    float bestDistance = INFINITY;
    for (int x = 0; x < land1.size(); x++)
    for (int y = 0; y < land2.size(); y++) {
        float dist = basic::distancef(land1[x]->pos, land2[y]->pos);
        if (dist < bestDistance) {
            bestDistance = dist;
            city1 = land1[x];
            city2 = land2[y];
        }
    }
    
    return std::make_pair(city1, city2);
}

void map::generation(int width, int height, map::world* world) {
    //-------Generate Cities-------
    std::cout << "Generating Cities" << std::endl;
    for (int c = 0; c < map::numOfCities; c++) {
        map::city* current_city;
        current_city = &world->cities[c];
        current_city->pos.x = rand()%(width-map::borderOffset)+map::borderOffset;
        current_city->pos.y = rand()%(height-map::borderOffset)+map::borderOffset;
    }
    bool clear = false;
    while (!clear) {
        clear = recursionCheck(width, height, world);
    }
    //-------Generate Bridges-------
    std::cout << "Generating Bridges" << std::endl;
    for (int c_1 = 0; c_1 < map::numOfCities; c_1++) {
        map::city* city_1 = &world->cities[c_1];
        for (int c_2 = 0; c_2 < map::numOfCities; c_2++) {
            map::city* city_2 = &world->cities[c_2];
            if (city_1 == city_2)
                continue;
            float dist = basic::distancef(city_1->pos, city_2->pos);
            if (dist < map::distanceBridges) {
                city_1->bridges.push_back(city_2);
            }
        }
    }
    //-------Fix Gaps-------
    std::cout << "Fix Gaps" << std::endl;
    std::vector <city*> checked;
    for (int c = 0; c < map::numOfCities; c++) {
        bool newLand = true;
        if (InVector(&world->cities[c], checked)) {
            newLand = false;
        }
        if (newLand) {
            std::cout << "1\n";
            std::vector <city*> newCheck = setColor(c, world);
            std::cout << "2\n";
            std::pair <map::city*, map::city*> closest = getClosestnewLand(checked, newCheck);

            std::cout << "3\n";
            closest.first->bridges.push_back(closest.second);
            std::cout << "3.5\n";
            closest.second->bridges.push_back(closest.first);
            std::cout << "4\n";
            checked.insert(checked.begin(), newCheck.begin(), newCheck.end());
            std::cout << "5\n";
        }
    }
}
void map::city::drawCity(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);

    fillCircle(renderer, pos.x, pos.y, 10);
}
void map::city::drawBridges(SDL_Renderer* renderer) {
    for (int i = 0; i < bridges.size(); i++) {
        map::city* current_bridgedCity = bridges[i];
        SDL_RenderDrawLine(renderer, current_bridgedCity->pos.x, current_bridgedCity->pos.y, pos.x, pos.y);
    }
}
void map::MAPinit() {
    srand(time(NULL));
}