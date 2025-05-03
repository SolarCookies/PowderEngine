#include "ConcreteElement.h"
#include "PowderGrid.h"

void ConcreteElement::update(int x, int y, PowderGrid& grid)
{
	tickCounter = 1000;
}

sf::Color ConcreteElement::getColor()
{
	return sf::Color(128, 128, 128, 255);
}
