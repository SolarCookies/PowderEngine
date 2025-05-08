#pragma once
#include "PowderElement.h"

class Gas : public PowderScript {
public:
    Gas() {
        ID = 2;
        Name = "Gas";
        initColor = sf::Color(255, 255, 0, 255);
    }

    void OnConstruct(Element* element) override {
        element->ID = ID;
        element->Color = initColor;
        element->Sleep = 0;
        element->Density = 0.0f;
        element->HasDensity = true;
    }

    void Tick(int x, int y, int& frame, Element* element, PowderGrid& grid) override;

    void SafeTick(int x, int y, int& frame, Element* element, PowderGrid& grid) override;

};