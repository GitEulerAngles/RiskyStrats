#include <iostream>
#include "map.h"

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
        //If the city is off-screen or near off-screen then move it back
        normalizePosition(width, height, &city_1->pos);
        for (int c_2 = 0; c_2 < map::numOfCities; c_2++) {
            if (c_1 == c_2)
                continue;

            map::city* city_2 = &world->cities[c_2];

            //If two cities are ontop of eachother then remove city2
            if (round(city_1->pos.x) == round(city_2->pos.x) && round(city_1->pos.y) == round(city_2->pos.y)) {
                collisionCheck = false;
                city_2->pos.x = rand()%width;
                city_2->pos.y = rand()%height;
            }

            float dist = basic::distancef(city_1->pos, city_2->pos);

            //If two cities are too close to eachother then move them apart using trignometry functions
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
bool map::InVector(map::city* city, std::vector <map::city*> cities) {
    for (int i = 0; i < cities.size(); i++)
        if (city == cities[i])
            return true;
    return false;
}
bool map::InVector(unsigned int cityID, std::vector <unsigned int> citiesID) {
    for (int i = 0; i < citiesID.size(); i++)
        if (cityID == citiesID[i])
            return true;
    return false;
}

std::vector<map::city*> getContinent(int ID, map::world* world) {
    //Two vectors. One for children that need their neighboring cities checked,
    //The other vector for cities that have already had their neighboring cities checked.
    std::vector <map::city*> open, closed;
    open.insert(open.begin(), world->cities[ID].bridges.begin(), world->cities[ID].bridges.end());
    closed.push_back(&world->cities[ID]);

    while (open.size() != 0) {
        int deleteTo = open.size();

        //Puts all open cities's neighbors into the open vector to be checked later.
        for (int i = 0; i < deleteTo; i++)
            for (int y = 0; y < open[i]->bridges.size(); y++)
                if (!InVector(open[i]->bridges[y], closed) && !InVector(open[i]->bridges[y], open))
                    open.push_back(open[i]->bridges[y]);

        //Closes off any open cities that have been checked.
        closed.insert(closed.begin(), open.begin(), open.begin()+deleteTo);
        open.erase(open.begin(), open.begin()+deleteTo);
    }

    return closed;
}
std::vector<map::city*> removeVectorFromVector(std::vector <map::city*> toRemove, std::vector <map::city*> cities) {
    //Remove all elements that are equal to any element from another vector
    std::vector<map::city*> newCities;

    for (int c = 0; c < cities.size(); c++) {
        bool isInside = false;
        for (int r = 0; r < toRemove.size(); r++) {
            if (toRemove[r] == cities[c]) {
                isInside = true;
                break;
            }
        }
        if (!isInside) 
            newCities.push_back(cities[c]);
    }

    return newCities;
}
std::pair<int, int> getClosestnewLand(std::vector <map::city*> land1, std::vector <map::city*> land2) {
    int city1ID, city2ID;
    float bestDistance = INFINITY;

    //Iterates through all cities in each continent to see which city has the closest distance.
    for (int x = 0; x < land1.size(); x++)
    for (int y = 0; y < land2.size(); y++) {
        float dist = basic::distancef(land1[x]->pos, land2[y]->pos);
        if (dist < bestDistance) {
            bestDistance = dist;
            city1ID = land1[x]->id;
            city2ID = land2[y]->id;
        }
    }
    
    return std::make_pair(city1ID, city2ID);
}

void generateBridges(map::world* world) {
    for (int c_1 = 0; c_1 < map::numOfCities; c_1++)
    for (int c_2 = 0; c_2 < map::numOfCities; c_2++) {
        map::city* city_1 = &world->cities[c_1];
        map::city* city_2 = &world->cities[c_2];
        if (city_1 == city_2)
            continue;
        float dist = basic::distancef(city_1->pos, city_2->pos);
        if (dist < map::distanceBridges)
            city_1->bridges.push_back(city_2);
    }
}
bool fixGaps(map::world* world) {
    //-------See if Connected-------
    if (getContinent(0, world).size() == map::numOfCities)
        return true;
    //-------See Continents-------
    std::vector <map::city*> unchecked;
    std::vector <std::vector <map::city*> > checked;
    for (int c = 0; c < map::numOfCities; c++)
        unchecked.push_back(&world->cities[c]);
    while (unchecked.size() != 0) {
        checked.push_back(getContinent(unchecked[0]->id, world));
        unchecked = removeVectorFromVector(checked[checked.size()-1], unchecked);
    }

    //-------Fix Gaps-------
    std::vector <map::city*> allCities;
    for (int c = 0; c < map::numOfCities; c++)
        allCities.push_back(&world->cities[c]);
    for (int c = 0; c < checked.size(); c++) {
        std::vector <map::city*> otherChecked = removeVectorFromVector(checked[c], allCities);
        std::pair <int, int> closestCities = getClosestnewLand(checked[c], otherChecked);
        if (!InVector(&world->cities[closestCities.first], world->cities[closestCities.second].bridges)) {
            world->cities[closestCities.first].bridges.push_back(&world->cities[closestCities.second]);
            world->cities[closestCities.second].bridges.push_back(&world->cities[closestCities.first]);
        }
    }

    return false;
}

void map::generation(int width, int height, map::world* world) {
    srand(time(NULL));

    std::cout << "Generating Cities" << std::endl;
    for (int c = 0; c < map::numOfCities; c++) {
        map::city* current_city;
        current_city = &world->cities[c];
        current_city->pos.x = rand()%(width-map::borderOffset)+map::borderOffset;
        current_city->pos.y = rand()%(height-map::borderOffset)+map::borderOffset;
        current_city->id = c;
        current_city->team = 0;
    }
    bool clear = false;
    while (!clear) {
        clear = recursionCheck(width, height, world);
    }

    std::cout << "Generating Teams" << std::endl;
    int r = rand()%map::numOfCities;
    world->cities[r].team = 1;
    world->cities[r].amount = 9999;

    std::cout << "Generating Bridges" << std::endl;
    generateBridges(world);
    clear = false;

    std::cout << "Fixing Gaps" << std::endl;
    while (!clear) {
        clear = fixGaps(world);
    }
}