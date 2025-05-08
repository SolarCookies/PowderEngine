#include "Gas.h"
#include "../World/PowderGrid.h"

void Gas::Tick(int x, int y, int& frame, Element* element, PowderGrid& grid)
{
    element->DirtyTick = false;

    int directionX = Rand::Int(element->Seed + frame, -1, 1);
    int directionY = Rand::Int(element->Seed / 2  + frame, -1, 1);

    if (grid.isEmpty(x + directionX, y + directionY)) {
        grid.move(x, y, x + directionX, y + directionY);
    }
}

void Gas::SafeTick(int x, int y, int& frame, Element* element, PowderGrid& grid)
{
    int directionX = Rand::Int(element->ID + frame, -1, 1);
    int directionY = Rand::Int(element->ID * 2 + frame, -1, 1);

    if (grid.isEmpty(x + directionX, y + directionY)) {
        element->DirtyTick = true;
    }
}
