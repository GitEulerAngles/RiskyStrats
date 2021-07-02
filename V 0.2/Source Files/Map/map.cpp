#include "SDL.h"
#include "map.h"
#include "player.h"

basic::text t;

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
float troopsCarryingCapacity(int amount) {
    const float L = 4;
    const float k = 0.0001;
    const float b = 100;
    const float e = 2.71828;
    float speed = -L/(1+pow(e, -k*(amount-b)))+L;
    if (speed < .1)
        return .1;
    return speed;
}

void map::MAPinit() {
    t.Color ={ 255,255,255 };
    t.Font = TTF_OpenFont("Fonts/Square.ttf", 14);
}

map::troops::troops(int _amount, basic::Vector2f _pos) {
    amount = _amount;
    pos = _pos;
    speed = troopsCarryingCapacity(amount);
}

void map::updateTroops(world* world) {
    for (int i = world->troops.size()-1; i >= 0; i--)
        if (world->troops[i].update())
            world->troops.erase(world->troops.begin()+i);
}

bool map::troops::update() {
    if (basic::distancef(pos, goTo->pos) < 5) {
        goTo->amount += amount;
        return true;
    }
    float rad = atan2f(pos.y - goTo->pos.y, pos.x - goTo->pos.x);
    pos.x -= cos(rad) * speed;
    pos.y -= sin(rad) * speed;
    return false;
}

void map::troops::draw(SDL_Renderer* renderer) {
    int size = log(amount);
    if (size < 2)
        size = 2;
    fillCircle(renderer, pos.x, pos.y, round(size));
}

void map::updateCities(world* world) {
    static basic::timer t;
    if (t.getTime() > 300) {
        for (int i = 0; i < map::numOfCities; i++)
            if (world->cities[i].amount >= 500)
                world->cities[i].amount += 1;
        t.restartTime();
    }
}

void map::city::update() {
    amount += 1;
}

void map::world::getSelectedInput(const Uint8* keystates, ply::player* player) {
    for (int c = 0; c < map::numOfCities; c++) {
        if (basic::distancef(basic::Vector2f((float)player->mousePos.x, (float)player->mousePos.y), cities[c].pos) < cities[c].popularity) {
                player->hovered_cityID = c;
            break;
        }
    }

    if (player->right && keystates[SDL_SCANCODE_LSHIFT]) {
        if (!InVector(player->hovered_cityID, player->selected_cityID))
            player->selected_cityID.insert(player->selected_cityID.begin(), player->hovered_cityID);
    }
    
    else if (player->right) {
        player->selected_cityID.clear();
        player->selected_cityID.insert(player->selected_cityID.begin(), player->hovered_cityID);
    }


}

void map::world::setTroopsInput(ply::player* player, map::city* selected_city) {
    int amountOfTroops = 0;

    if (!map::InVector(&cities[player->hovered_cityID], selected_city->bridges) || !player->keyPress)
        return;

    switch (player->troopsButton)
    {
    case 5:
        amountOfTroops = selected_city->amount - 500;
        break;
    default:
        amountOfTroops = player->troopsSize[player->troopsButton];
        break;
    }

    if (selected_city->amount < amountOfTroops)
        amountOfTroops = selected_city->amount;

    if (amountOfTroops <= 0)
        return;

    selected_city->amount -= amountOfTroops;
    map::troops newTroops(amountOfTroops, selected_city->pos);
    newTroops.goTo = &cities[player->hovered_cityID];
    troops.insert(troops.begin(), newTroops);
}

void map::world::getTroopsInput(const Uint8* keystates, ply::player* player) {
    player->keyPress = false;
    static bool troopsKeys[6];
    const int keyboardCodes[6] ={ SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_W};
    for (int i = 0; i < 6; i++) {
        if (keystates[keyboardCodes[i]] && !troopsKeys[i]) {
            player->troopsButton = i;
            player->keyPress = true;
            troopsKeys[i] = true;
        }
        else if (!keystates[keyboardCodes[i]])
            troopsKeys[i] = false;
    }
}

void map::city::drawCity(SDL_Renderer* renderer, ply::player* player) {
    if (map::InVector(this->id, player->selected_cityID))
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    else if (player->hovered_cityID != -1 && player->hovered_cityID == id)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    else 
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
    int log = ceil(log10(amount));
    if (amount <= 1)
        log = 1;
    popularity = log * 2 + 3;
    fillCircle(renderer, pos.x, pos.y, popularity);
    SDL_Rect r;
    int big = popularity * 4;
    basic::Vector2i dimOfText;
    int size = TTF_SizeText(t.Font, std::to_string(amount).c_str(), &dimOfText.x, &dimOfText.y);
    r.x = pos.x - (dimOfText.x / 2.0f);
    r.y = pos.y - (popularity + dimOfText.y);
    r.w = dimOfText.x;
    r.h = dimOfText.y;
    t.drawText(renderer, std::to_string(amount), r);
}

void map::city::drawBridges(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < bridges.size(); i++) {
        map::city* current_bridgedCity = bridges[i];
        SDL_RenderDrawLine(renderer, current_bridgedCity->pos.x, current_bridgedCity->pos.y, pos.x, pos.y);
    }
}