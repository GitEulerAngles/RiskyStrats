#ifndef PLAYER
#define PLAYER

#include "basic.h"
#include <vector>

namespace ply {
    class player {
    public:
        void getPlayerInput(bool* isGameRunning);
        unsigned int hovered_cityID = 1;
        unsigned int troopsButton = 0;
        unsigned int troopsSize[5] ={ 10,100,500,1000,10000 };
        std::vector <unsigned int> selected_cityID;
        basic::Vector2i mousePos;
        bool left, right, middle, keyPress;
    private:
    };
}

#endif //!PLAYER
