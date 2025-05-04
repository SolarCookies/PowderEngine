#include "WaterElement.h"
#include "../PowderGrid.h"
#include <random>



void WaterElement::update(int x, int y, PowderGrid& grid) {
    if (auto below = grid.get(x, y + 1); below && below->isEmpty()) {
        grid.move(x, y, x, y + 1);
        return;
    }

    if (auto above = grid.get(x, y - 1); above && above->getDensity() > getDensity() && !above->isSolid()) {
        grid.swap(x, y, x, y - 1);
        return;
    }

    seed = (x + grid.frame + id) * (y + grid.frame + id);
    randomDirection = getRandomInt(seed, 0, 1);

    // Helper lambda to reduce redundancy
    auto tryMove = [&](int dx, int dy) -> bool {
        if (auto target = grid.get(x + dx, y + dy); target && target->isEmpty()) {
            grid.move(x, y, x + dx, y + dy);
            return true;
        }
        return false;
        };

    if (randomDirection == 0) {
        // Try moving left (Less in order to try to bypass grid execution bias)
        if (tryMove(-1, 1) || tryMove(-1, 0) || tryMove(-2, 0) || tryMove(-1, 2)) {
            return;
        }
    }
    else {
        // Try moving right
        if (tryMove(1, 1) || tryMove(1, 0) || tryMove(2, 0) || tryMove(3, 0) || tryMove(4, 0) || tryMove(1, 2)) {
            return;
        }
    }
}


sf::Color WaterElement::getColor()
{
	if (color != sf::Color(0, 0, 0)) {
		return color;
	}
	color = sf::Color(0, 0, 255);
	return color;
}


