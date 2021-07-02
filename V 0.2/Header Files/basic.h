#ifndef BASIC
#define BASIC

#include "SDL.h"
#include "SDL_ttf.h"
#include <chrono>
#include <cstdint>
#include <iostream>

namespace basic {
    class text {
    public:
        SDL_Color Color;
        TTF_Font* Font;

        void drawText(SDL_Renderer* r, std::string message, SDL_Rect rect) {
            surfaceMessage = TTF_RenderText_Solid(Font, message.c_str(), Color);
            Message = SDL_CreateTextureFromSurface(r, surfaceMessage);
            SDL_RenderCopy(r, Message, NULL, &rect);
            SDL_FreeSurface(surfaceMessage);
            SDL_DestroyTexture(Message);
        }
        ~text() {
            TTF_CloseFont(Font);
        }
    private:
        SDL_Surface* surfaceMessage;
        SDL_Texture* Message;
    };

    struct Vector2f {
        float x;
        float y;
        Vector2f(float sx, float sy) {
            x = sx;
            y = sy;
        }
        Vector2f() {
        };
    };

    struct Vector2i {
        int x;
        int y;
        Vector2i(int sx, int sy) {
            x = sx;
            y = sy;
        }
        Vector2i() {
        };
    };

    struct Color {
        uint8_t  r;
        uint8_t  g;
        uint8_t  b;
        Color(uint8_t Red, uint8_t Green, uint8_t Blue) {
            r = Red;
            g = Green;
            b = Blue;
        }
        Color() {
        };
    };

    struct timer {
        std::chrono::time_point<std::chrono::steady_clock> start, end;
        std::chrono::duration<float> duration;

        void restartTime() {
            start = std::chrono::high_resolution_clock::now();
        }

        timer() {
            restartTime();
        }

        float getTime() {
            end = std::chrono::high_resolution_clock::now();

            duration = end - start;

            return duration.count()*1000;
        }

        float getFPS() {
            return 1/(getTime() / 1000);
        }

        float getFPS(float Time) {
            return 1/(Time / 1000);
        }
    };

    basic::Vector2i addVector2i(basic::Vector2i v1, basic::Vector2i v2);
    basic::Vector2i subVector2i(basic::Vector2i v1, basic::Vector2i v2);
    float distancei(basic::Vector2i p1, basic::Vector2i p2);
    float distancef(basic::Vector2f p1, basic::Vector2f p2);
};



#endif //BASIC