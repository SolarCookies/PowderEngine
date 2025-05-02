#include "SandElement.h"
#include "EmptyElement.h"
#include "PowderGrid.h"

#include <random>

void SandElement::update(int x, int y, PowderGrid& grid) {
		
    if (auto below = grid.get(x, y + 1); below && below->isEmpty()) {

        // Move the current SandElement to the new position
		grid.move(x, y, x, y + 1); // Move the current element to the new position
	}
	else {
		//Randomly decide to move left or right
		seed = x * y + grid.frame; // Use the position as a seed for randomness
		int randomDirection = getRandomInt(seed, 0, 1); // Randomly choose between 0 and 1

		if (randomDirection == 0) {
			// Move left
			if (auto belowLeft = grid.get(x - 1, y + 1); belowLeft && belowLeft->isEmpty()) {
				// Move the current SandElement to the new position
				grid.move(x, y, x - 1, y + 1); // Move the current element to the new position
			}
		}
		else {
			// Move right
			if (auto belowRight = grid.get(x + 1, y + 1); belowRight && belowRight->isEmpty()) {
				// Move the current SandElement to the new position
				grid.move(x, y, x + 1, y + 1); // Move the current element to the new position
			}
		}

		//Wasnt able to move, Sleep for 5 ticks
		tickCounter = 5;
	}
}

sf::Color SandElement::getColor()
{
	if (color != sf::Color(0, 0, 0)) {
		if (tickCounter == 1) {
			return color;
		}
		else {
			//red fades to the normal color from 5 being full red to 0 being black
			return color;
			int r = 255 - (255 * (tickCounter / 5));
			return sf::Color(r, 0, 0);
		}
	}
	//random color
	std::random_device rd;  // Obtain a random number from hardware
	std::mt19937 gen(rd()); // Seed the generator
	std::uniform_int_distribution<> dis(150, 255);
	color = sf::Color(dis(gen), dis(gen), dis(gen));
	return color;

}
