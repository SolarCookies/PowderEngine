#pragma once
#include "PowderElement.h"

class Life : public PowderScript {
public:
    Life() {
        ID = 3;
        Name = "Life";
        initColor = sf::Color(0, 255, 0, 255);
    }

    void OnConstruct(Element* element) override {
        element->ID = ID;
        element->Color = initColor;
        element->Sleep = 0;
        element->Density = 0.0f;
        element->HasDensity = false;
    }

    void Tick(int x, int y, int& frame, Element* element, PowderGrid& grid) override;

    void SafeTick(int x, int y, int& frame, Element* element, PowderGrid& grid) override;

private:
    bool isLife(Element* e) const;
    int countLivingNeighbors(int x, int y, PowderGrid& grid) const;
};