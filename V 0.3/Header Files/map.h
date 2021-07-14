#ifndef MAP
#define MAP

#include "basic.h"
#include "SDL.h"
#include "player.h"
#include <random>
#include <vector>

namespace map {
    const int numOfCities = 250;
    const int borderOffset = 50;
    const int distanceBetweenCities = 50;
    const int distanceBridges = 90;

    struct city {
        unsigned int id, team, popularity;
        unsigned int amount = 500;

        basic::Vector2f pos;
        basic::timer t;
        std::vector <city*> bridges;
        std::vector <std::pair <unsigned int, unsigned int>> attackers;

        void drawBridges(SDL_Renderer* renderer);
        void drawCity(SDL_Renderer* renderer, ply::player* player, basic::text* t);
        void update();
    };

    struct troops {
        float speed;
        unsigned int amount, team;

        basic::Vector2f pos;
        city* goTo;

        bool update();
        void draw(SDL_Renderer* renderer);

        troops(unsigned int _amount, unsigned int _team, basic::Vector2f _pos);
    };

    class world {
    public:
        basic::text troopsText;
        std::vector <troops> troops;
        city cities[numOfCities];

        void getTroopsInput(const Uint8* keystates, ply::player* player);
        void setTroopsInput(ply::player* player, map::city* selected_city);
        void getSelectedInput(const Uint8* keystates, ply::player* player);
    };
    bool InVector(map::city* city, std::vector <map::city*> cities);
    bool InVector(unsigned int cityID, std::vector <unsigned int> citiesID);
    void updateCities(world* world);
    void updateTroops(world* world);
    void generation(int width, int height, world* world);
    void MAPinit(basic::text* t);

}

#endif //!MAP