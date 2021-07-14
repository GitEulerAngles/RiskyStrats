#ifndef COMPUTER
#define COMPUTER

#include "map.h"

namespace ai {
    class computer {
    public:
        unsigned int team = 1;
        void updateComputer(map::world* world);
    };
}

#endif //COMPUTER