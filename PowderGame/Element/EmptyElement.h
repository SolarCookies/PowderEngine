#pragma once
#include "PowderElement.h"

class EmptyElement : public PowderElement {
public:
	std::unique_ptr<PowderElement> clone() override { return std::make_unique<EmptyElement>(*this); }
    sf::Color GetColor(std::string Name) override { return sf::Color(0,0,0,0); }
    bool isEmpty() const override { return true; }
	
};