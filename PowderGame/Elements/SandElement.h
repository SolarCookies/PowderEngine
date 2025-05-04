#pragma once
#include "PowderElement.h"

class SandElement : public PowderElement {
public:
    void update(int x, int y, PowderGrid& grid) override;
	std::unique_ptr<PowderElement> clone() const override { return std::make_unique<SandElement>(*this); }
    sf::Color getColor() override;
	std::string GetName() const override { return "Sand"; }
    sf::Color color = sf::Color(0, 0, 0);
	int getDensity() override { return 1; } // Sand density
};