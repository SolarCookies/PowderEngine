#pragma once
#include "PowderElement.h"

class ConcreteElement : public PowderElement {
public:
    void update(int x, int y, PowderGrid& grid) override;
    std::unique_ptr<PowderElement> clone() const override { return std::make_unique<ConcreteElement>(*this); }
    sf::Color getColor() override;
    std::string GetName() const override { return "Wall"; }
    sf::Color color = sf::Color(0, 0, 0, 255);
    bool isSolid() const override { return true; } // Concrete is solid (Cant be swapped by water)
    bool isEmpty() const override { return false; }
};