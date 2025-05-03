#pragma once
#include "PowderElement.h"

class EmptyElement : public PowderElement {
public:
    void update(int x, int y, PowderGrid& grid) override {}
	std::unique_ptr<PowderElement> clone() const override { return std::make_unique<EmptyElement>(*this); }
    sf::Color getColor() override { return sf::Color(0,0,0,0); }
    bool isEmpty() const override { return true; }
	int getDensity() override { return -1; }
	std::string GetName() const override { return "Empty"; }
	bool isSolid() const override { return false; }
	
};