#ifndef _MOVING_OBJECT_H
#define _MOVING_OBJECT_H

#include "Object.h"

enum Direction {
	UP,
    DOWN,
    LEFT,
    RIGHT
};

class MovingObject : public Object
{
	Direction direction;
    int moveInterval;

    void UpdatePosition(int& start, int end);
public:
	MovingObject();

    bool Update();
    void SetDirection(Direction direction);
	void SetMoveInterval(int moveInterval);
    Direction GetDirection();
	Square* GetNextSquare();
};

#endif