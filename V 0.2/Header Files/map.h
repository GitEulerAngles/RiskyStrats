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
        unsigned int id;
        int amount = 100;
        int popularity;
        basic::Color c ={ 0,255,255 };
        basic::Vector2f pos;
        std::vector <city*> bridges;
        void update();
        void drawCity(SDL_Renderer* renderer, ply::player* player);
        void drawBridges(SDL_Renderer* renderer);
    };

    struct troops {
        float speed;
        int amount = 0;
        basic::Vector2f pos;
        city* goTo;
        bool update();
        void draw(SDL_Renderer* renderer);
        troops(int _amount, basic::Vector2f _pos);
    };

    class world {
    public:
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
    void MAPinit();

}

#endif //!MAP