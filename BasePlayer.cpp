#include "BasePlayer.h"
#include "Game.h"
#include "DroppingObject.h"
#include "RegularArrow.h"
#include "ExplodingArrow.h"
#include "PenetratingArrow.h"
#include <iostream>
#include <exception>

using namespace std;

const int BasePlayer::INITIAL_POWER                   = 1000;
const int BasePlayer::MIN_TICKS_BETWEEN_ARROWS        = 3;
const int BasePlayer::SHOOT_ARROW_PROBABILITY         = 20;
const int BasePlayer::CHANGE_DIRECTION_PROBABILITY    = 10;
const int BasePlayer::MOVE_INTERVAL                   = 2;

BasePlayer::BasePlayer(char name) {
    this->name = name;
    color = Console::WHITE;
    power = INITIAL_POWER;
    direction = RIGHT;
    lastArrowTick = 0;
}

BasePlayer::~BasePlayer() {
    if (pGame->isRunning()) {
        // Clear square before deletion
        pSquare->stepOut(*this);
    }
}

void BasePlayer::setSquare(Grid::Square& square) {
    if (square.hasDroppingObject()) {
        // Pick up dropping object
        DroppingObject& droppingObject = square.getDroppingObject();
        droppingObject.affect(*this);
    }

    ObjectsList& players = square.getPlayers();
    if (!players.empty()) {
        // Fight other players on square
        ObjectsIterator it = players.begin();
        while (it != players.end()) {
            BasePlayer* player = (BasePlayer*) *it;
            fight(*player);
            it++;
        }
    }

    if (pSquare) {
        pSquare->stepOut(*this);
    }
    square.stepIn(*this);
    pSquare = &square;
}

void BasePlayer::setColor(Console::Color color) {
    this->color = color;
}

bool BasePlayer::shootArrow(ArrowsBag::Type type) {
    Grid::Square& square = getNextSquare();
    if (
        arrowsBag.remaining[type] > 0 &&                            // Player still has arrows
        pGame->getTick() > lastArrowTick + MIN_TICKS_BETWEEN_ARROWS // Check minimum ticks between arrows
    ) {
        BaseArrow* arrow = arrowsBag.getArrow(type);
        arrow->setDirection(direction);
        pGame->addArrow(arrow, square); // Update game
        lastArrowTick = pGame->getTick();
        return true;
    }
    return false;
}

void BasePlayer::fight(BasePlayer& opponent) {
    if (power > opponent.getPower()) {
        // Player is stronger than opponent
        opponent.decreasePower(200);
        decreasePower(10);
    } else if (power < opponent.getPower()) {
        // Player is weaker than opponent
        opponent.decreasePower(10);
        decreasePower(200);
    } else {
        // Player and opponent are equally matched
        opponent.decreasePower(50);
        decreasePower(50);
    }
}

void BasePlayer::increasePower(int amount) {
    power = __max(power + amount, 0);
}

void BasePlayer::decreasePower(int amount) {
    increasePower(-amount);
}

Console::Color BasePlayer::getColor() const {
    return color;
}

char BasePlayer::getName() const {
    return name;
}

int BasePlayer::getPower() const {
    return power;
}

BasePlayer::ArrowsBag& BasePlayer::getArrowsBag() {
    return arrowsBag;
}

void BasePlayer::draw() const {
    pSquare->draw(name, color);
}

ostream& operator<<(ostream& out, const BasePlayer& player) {
    Console::changeColor(player.color);
    printf("%4d", player.power);
    return cout << player.arrowsBag;
}

// Player arrows bag

const int BasePlayer::ArrowsBag::INITIAL_NUM_REGULAR        = 2;
const int BasePlayer::ArrowsBag::INITIAL_NUM_EXPLODING      = 1;
const int BasePlayer::ArrowsBag::INITIAL_NUM_PENETRATING    = 1;

BasePlayer::ArrowsBag::ArrowsBag() {
    remaining[REGULAR] = INITIAL_NUM_REGULAR;
    remaining[EXPLODING] = INITIAL_NUM_EXPLODING;
    remaining[PENETRATING] = INITIAL_NUM_PENETRATING;
}

bool BasePlayer::ArrowsBag::isEmpty() const {
    return (remaining[REGULAR] + remaining[EXPLODING] + remaining[PENETRATING] == 0);
}

int BasePlayer::ArrowsBag::getRemaining(Type type) const {
    return remaining[type];
}

BasePlayer::ArrowsBag::Type BasePlayer::ArrowsBag::getAvailableRandomType() const {
    if (isEmpty()) {
        throw runtime_error("Arrows bag is empty");
    }

    // Check which arrow type is available
    int available[3], numAvailable = 0;
    for (int type = 0; type < 3; type++) {
        if (remaining[type] > 0) {
            available[numAvailable] = type;
            numAvailable++;
        }
    }

    // Choose randomly from available types
    int random = rand() % numAvailable;
    return (Type) available[random];
}

BaseArrow* BasePlayer::ArrowsBag::getArrow(Type type) {
    BaseArrow* arrow = NULL;
    if (remaining[type] > 0) {
        // Allocate arrow
        switch (type) {
            case REGULAR:
                arrow = new RegularArrow;
                break;
            case EXPLODING:
                arrow = new ExplodingArrow;
                break;
            case PENETRATING:
                arrow = new PenetratingArrow;
                break;
        }

        // Decrease counter
        remaining[type]--;
    }
    return arrow;
}

BasePlayer::ArrowsBag& BasePlayer::ArrowsBag::operator+=(int amount) {
    remaining[REGULAR] += amount;
    remaining[EXPLODING] += amount;
    remaining[PENETRATING] += amount;
    return *this;
}

BasePlayer::ArrowsBag& BasePlayer::ArrowsBag::operator++() {
    // Add one arrow of each type
    return (*this += 1);
}

ostream& operator<<(ostream& out, const BasePlayer::ArrowsBag& arrowsBag) {
    printf(
        "%2d%2d%2d",
        arrowsBag.remaining[BasePlayer::ArrowsBag::REGULAR],
        arrowsBag.remaining[BasePlayer::ArrowsBag::EXPLODING],
        arrowsBag.remaining[BasePlayer::ArrowsBag::PENETRATING]
    );
    return out;
}