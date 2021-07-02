#ifndef MAP
#define MAP

#include "basic.h"
#include "SDL.h"
#include <random>
#include <vector>

namespace map {
    const int numOfCities = 50;
    const int borderOffset = 50;
    const int distanceBetweenCities = 50;
    const int distanceBridges = 100;
    struct troops {
        int amount = 0;
        basic::Vector2f pos;
        troops(int _amount, basic::Vector2f _pos) {
            amount = _amount;
            pos = _pos;
        }
    };

    struct city {
        basic::Color c ={255,255,255};
        int id;
        int amount = 0;
        basic::Vector2f pos;
        std::vector <city*> bridges;
        void drawCity(SDL_Renderer* renderer);
        void drawBridges(SDL_Renderer* renderer);
    };

    class world {
    public:
        std::vector <std::vector <city*> > color;
        city cities[numOfCities];
    };

    void generation(int width, int height, world* world);
    void MAPinit();
}

#endif //!MAP