#include "Life.h"
#include "../World/PowderGrid.h"

bool Life::isLife(Element* e) const {
    if (!e) return false;
    return e->ID == 3;
}

int Life::countLivingNeighbors(int x, int y, PowderGrid& grid) const {
    int count = 0;
    for (int ox = -1; ox <= 1; ++ox) {
        for (int oy = -1; oy <= 1; ++oy) {
            if (ox == 0 && oy == 0) continue;
            if (isLife(grid.get(x + ox, y + oy))) count++;
        }
    }
    return count;
}

void Life::Tick(int x, int y, int& frame, Element* element, PowderGrid& grid) {
    element->DirtyTick = false;

    int aliveNeighbors = countLivingNeighbors(x, y, grid);

    if (aliveNeighbors < 2 || aliveNeighbors > 3) {
        grid.clearElement({ x, y });
        return;
    }

    for (int ox = -1; ox <= 1; ++ox) {
        for (int oy = -1; oy <= 1; ++oy) {
            if (ox == 0 && oy == 0) continue;

            int nx = x + ox;
            int ny = y + oy;

            if (!grid.isEmpty(nx, ny)) continue;

            int neighbors = countLivingNeighbors(nx, ny, grid);
            if (neighbors == 3) {
                grid.set(nx, ny, *element);
            }
        }
    }
}

void Life::SafeTick(int x, int y, int& frame, Element* element, PowderGrid& grid) {
    int aliveNeighbors = countLivingNeighbors(x, y, grid);

    if (aliveNeighbors < 2 || aliveNeighbors > 3) {
        element->DirtyTick = true;
        return;
    }

    for (int ox = -1; ox <= 1; ++ox) {
        for (int oy = -1; oy <= 1; ++oy) {
            if (ox == 0 && oy == 0) continue;

            int nx = x + ox;
            int ny = y + oy;

            if (!grid.isEmpty(nx, ny)) continue;

            int neighbors = countLivingNeighbors(nx, ny, grid);
            if (neighbors == 3) {
                element->DirtyTick = true;
                return;
            }
        }
    }
}