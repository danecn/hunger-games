#include "Arrow.h"
#include "Game.h"
#include "Player.h"

Arrow::Arrow(Player& shooter, Grid::Square& square) {
    pSquare = &square;
    direction = shooter.getDirection();
    hit = false;
}

Arrow::~Arrow() {
    pSquare->clear();
}

void Arrow::setSquare(Grid::Square& square) {
    if (square.hasWall()) {
        hit = true;
    } else {
        pSquare = &square;
        checkHit();
    }
}

void Arrow::update(Game& game) {
    if (!checkHit() && game.getTick() % MOVE_INTERVAL == 0) {
        Grid::Square& nextSquare = getNextSquare(game.getGrid(), *pSquare, direction);
        pSquare->clear();
        setSquare(nextSquare);
    }
}

bool Arrow::checkHit() {
    if (pSquare) {
        const List& players = pSquare->getPlayers();
        if (!players.isEmpty()) {
            // Hit first player on square
            Player* player = (Player*) players.peek();
            player->decreasePower(500);
            hit = true;
        }
    }
    return hit;
}

void Arrow::draw() const {
    char ch;
    switch (direction) {
        case UP:
            ch = '^';
            break;
        case DOWN:
            ch = 'v';
            break;
        case LEFT:
            ch = '<';
            break;
        case RIGHT:
            ch = '>';
            break;
    }
    pSquare->draw(ch, WHITE);
}

bool Arrow::getHit() const {
    return hit;
}