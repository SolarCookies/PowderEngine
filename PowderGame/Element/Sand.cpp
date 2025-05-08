#include "Sand.h"
#include "../World/PowderGrid.h"

void Sand::BeginPlay(int x, int y, int& frame, Element* element)
{
    int baseR = 220;
    int baseG = 200;
    int baseB = 150;

    int offset = Rand::Int(element->Seed + frame, -10, 10);
    int r = Clamp(baseR + offset, 180, 255);
    int g = Clamp(baseG + offset, 160, 230);
    int b = Clamp(baseB + offset, 120, 210);

    element->Color = sf::Color(r, g, b, 255);
}

void Sand::Tick(int x, int y, int& frame, Element* element, PowderGrid& grid)
{
    element->DirtyTick = false;


    if (!grid.isEmpty(x, y + 1) && !grid.isEmpty(x + 1, y + 1) && !grid.isEmpty(x - 1, y + 1)) {
        if (grid.get(x, y + 1)->HasDensity &&
            grid.get(x, y + 1)->Density < element->Density) {
            grid.swap(x, y, x, y + 1);
        }
        element->Sleep = 5;
        return;
    }

    if (grid.isEmpty(x, y + 1)) {
        grid.move(x, y, x, y + 1);
        return;
    }
    else if (
        grid.get(x, y + 1)->HasDensity &&
        grid.get(x, y + 1)->Density < element->Density) {
        grid.swap(x, y, x, y + 1);
        return;
    }

    int direction = Rand::Int(element->ID + frame, 0, 1);
    int dx = direction == 0 ? 1 : -1;

    if (grid.isEmpty(x + dx, y + 1)) {
        grid.move(x, y, x + dx, y + 1);
        return;
    }
    else if (
        grid.get(x + dx, y + 1)->HasDensity &&
        grid.get(x + dx, y + 1)->Density < element->Density) {
        grid.swap(x, y, x + dx, y + 1);
        return;
    }
}

void Sand::SafeTick(int x, int y, int& frame, Element* element, PowderGrid& grid)
{

    if (!grid.isEmpty(x, y + 1) && !grid.isEmpty(x + 1, y + 1) && !grid.isEmpty(x - 1, y + 1)) {
        if (grid.get(x, y + 1)->HasDensity &&
            grid.get(x, y + 1)->Density < element->Density) {
            element->DirtyTick = true;
        }
        element->Sleep = 5;
        return;
    }

    if (grid.isEmpty(x, y + 1)) {
        element->DirtyTick = true;
        return;
    }
    else if (
        grid.get(x, y + 1)->HasDensity &&
        grid.get(x, y + 1)->Density < element->Density) {
        element->DirtyTick = true;
        return;
    }

    int direction = Rand::Int(element->ID + frame, 0, 1);
    int dx = direction == 0 ? 1 : -1;

    if (grid.isEmpty(x + dx, y + 1)) {
        element->DirtyTick = true;
        return;
    }
    else if (
        grid.get(x + dx, y + 1)->HasDensity &&
        grid.get(x + dx, y + 1)->Density < element->Density) {
        element->DirtyTick = true;
        return;
    }
}
