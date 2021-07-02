#include <iostream>
#include "window.hpp"
#include "basic.h"
#include <thread>

#undef main

void main() {
    window w("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, 60);

    float frameDelay = 1000 / w.FPS;
    float frameTime;

    std::cout << "Loading Setup!\n";
    w.setup();
    std::cout << "Setup Complete!\n";

    int t = 0;

    while (w.running) {
        basic::timer FPSTimer;

        w.input();
        w.update();
        w.render();

        frameTime = FPSTimer.getTime();

        if (frameDelay > frameTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(frameDelay - frameTime)));
        }
    }

    w.clean();
}