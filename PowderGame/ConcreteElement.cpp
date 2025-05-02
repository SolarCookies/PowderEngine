#include "ConcreteElement.h"
#include "PowderGrid.h"

void ConcreteElement::update(int x, int y, PowderGrid& grid)
{
	tickCounter = 1000;
}

void ConcreteElement::updateHeat(int x, int y, PowderGrid& grid)
{
	temprature = 100; // Start temperature of the element
	grid.setheat(x, y, 1);
	return;
}

sf::Color ConcreteElement::getColor()
{
	return sf::Color(128, 128, 128);
}
