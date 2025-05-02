#pragma once
#include "PowderElement.h"

class WaterElement : public PowderElement {
public:
    void update(int x, int y, PowderGrid& grid) override;
	std::unique_ptr<PowderElement> clone() const override { return std::make_unique<WaterElement>(*this); }
    sf::Color getColor() override;
    std::string GetName() const override { return "Water"; }
    sf::Color color = sf::Color(0, 0, 0);
	int getDensity() override { return 0; } // Water density

};