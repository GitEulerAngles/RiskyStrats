#include "basic.h"

basic::Vector2i basic::addVector2i(basic::Vector2i v1, basic::Vector2i v2) {
    return basic::Vector2i(v1.x + v2.x, v1.y + v2.y);
}
basic::Vector2i basic::subVector2i(basic::Vector2i v1, basic::Vector2i v2) {
    return basic::Vector2i(v1.x - v2.x, v1.y - v2.y);
}
float basic::distancei(basic::Vector2i p1, basic::Vector2i p2) {
    return sqrtf(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}
float basic::distancef(basic::Vector2f p1, basic::Vector2f p2) {
    return sqrtf(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}