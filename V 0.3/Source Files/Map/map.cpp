#include "SDL.h"
#include "map.h"
#include "player.h"

#define e 2.71828

basic::Color Colors[] ={
    {255,255,255},
    {255,255,153},
    {255,204,204}
};

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
int attackersInVector(unsigned int teamID, std::vector <std::pair <unsigned int, unsigned int>> attackers) {
    for (int i = 0; i < attackers.size(); i++)
        if (teamID == attackers[i].second)
            return i;
    return -1;
}

float citiesBellCurve(float amount) {
    const float a = 500000;
    const float b = 50000000000;
    const float c = 250;
    float children = -pow(e, -pow(amount-a, 2) / b) * c + 300;
    return children;
}
float troopsCarryingCapacity(float amount) {
    const float L = 4;
    const float k = 0.0001;
    const float b = 100;
    float speed = -L/(1+pow(e, -k*(amount-b)))+L;
    if (speed < .1)
        return .1;
    return speed;
}
float troopsIntoBattle(unsigned int army1, unsigned int army2) {
    float loss = powf(army1, 2) / powf(army2, 2);
    if ((float)army2 - loss <= 1)
        loss += (float)army2 - loss;
    return loss;
}

void map::MAPinit(basic::text* t) {
    t->Font = TTF_OpenFont("Fonts/Square.ttf", 14);
}
//Troops
void map::updateTroops(world* world) {
    for (int i = world->troops.size()-1; i >= 0; i--)
        if (world->troops[i].update())
            world->troops.erase(world->troops.begin()+i);
}

map::troops::troops(unsigned int _amount, unsigned int _team, basic::Vector2f _pos) {
    amount = _amount;
    pos = _pos;
    team = _team;
    speed = troopsCarryingCapacity(amount);
}
bool map::troops::update() {
    if (basic::distancef(pos, goTo->pos) < 5) {
        if (team == goTo->team)
            goTo->amount += amount;
        else {
            int t = attackersInVector(team, goTo->attackers);
            if (t == -1)
                goTo->attackers.insert(goTo->attackers.begin(), std::make_pair(amount, team));
            else
                goTo->attackers[t].first += amount;
        }
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
    SDL_SetRenderDrawColor(renderer, Colors[this->team].r, Colors[this->team].g, Colors[this->team].b, 255);
    fillCircle(renderer, pos.x, pos.y, round(size));
}
//Cities
void map::updateCities(world* world) {
    static basic::timer t;
    bool repopulate = false;

    if (t.getTime() >= 1000) {
        repopulate = true;
        t.restartTime();
    }

    for (int i = 0; i < map::numOfCities; i++) {
        //Update BirthRate
        if (world->cities[i].attackers.size() == 0) {
            float birthRate = citiesBellCurve(world->cities[i].amount);
            if (world->cities[i].t.getTime() >= birthRate && world->cities[i].amount >= 50) {
                world->cities[i].amount += 1;
                world->cities[i].t.restartTime();
            }
        }
        //Update attackers
        else if (repopulate) {
            world->cities[i].update();
        }
    }


}

void map::city::update() {
    for (int i = attackers.size()-1; i >= 0; i--) {
        attackers[i].first -= troopsIntoBattle(amount, attackers[i].first);
        amount -= troopsIntoBattle(attackers[i].first, amount);
        if (amount == 0) {
            team = attackers[i].second;
            amount = attackers[i].first;
            attackers.erase(attackers.begin()+i);
        }
        else if (attackers[i].first == 0)
            attackers.erase(attackers.begin()+i);
    }
}
void map::city::drawCity(SDL_Renderer* renderer, ply::player* player, basic::text* t) {
    //Draw Circle of City
    if (map::InVector(this->id, player->selected_cityID))
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    else if (player->hovered_cityID != -1 && player->hovered_cityID == id)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    else
        SDL_SetRenderDrawColor(renderer, Colors[this->team].r, Colors[this->team].g, Colors[this->team].b, 255);

    int log = ceil(log10(amount));
    if (amount <= 1)
        log = 1;
    popularity = log * 2 + 3;
    fillCircle(renderer, pos.x, pos.y, popularity);

    //Draw Text of City
    SDL_Rect r;
    basic::Vector2i dimOfText;
    TTF_SizeText(t->Font, std::to_string(amount).c_str(), &dimOfText.x, &dimOfText.y);
    r.x = pos.x - (dimOfText.x / 2.0f);
    r.y = pos.y - (popularity + dimOfText.y);
    r.w = dimOfText.x;
    r.h = dimOfText.y;
    t->Color ={Colors[this->team].r, Colors[this->team].g, Colors[this->team].b};
    t->drawText(renderer, std::to_string(amount), r);

    //Draw Text of Attackers
    for (int i = 0; i < attackers.size(); i++) {
        TTF_SizeText(t->Font, std::to_string(attackers[i].first).c_str(), &dimOfText.x, &dimOfText.y);
        r.x = pos.x - (dimOfText.x / 2.0f);
        r.y = pos.y - (popularity + dimOfText.y) - ( (i+1) * dimOfText.y);
        r.w = dimOfText.x;
        r.h = dimOfText.y;
        t->Color ={ Colors[attackers[i].second].r, Colors[attackers[i].second].g, Colors[attackers[i].second].b };
        t->drawText(renderer, std::to_string(attackers[i].first), r);
    }
}
void map::city::drawBridges(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < bridges.size(); i++) {
        map::city* current_bridgedCity = bridges[i];
        SDL_RenderDrawLine(renderer, current_bridgedCity->pos.x, current_bridgedCity->pos.y, pos.x, pos.y);
    }
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
    unsigned int amountOfTroops = 0;

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
    map::troops newTroops(amountOfTroops, selected_city->team, selected_city->pos);
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
