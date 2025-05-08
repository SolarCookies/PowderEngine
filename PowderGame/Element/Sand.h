#pragma once
#include "PowderElement.h"

class Sand : public PowderScript {
public:
    Sand() {
        ID = 1;
        Name = "Sand";
        initColor = sf::Color(235, 220, 180, 255);
    }

    void OnConstruct(Element* element) override {
        element->ID = 1;
        element->Color = initColor;
        element->Sleep = 0;
        element->HasDensity = true;
        element->Density = 2.0f;
    }

    void BeginPlay(int x, int y, int& frame, Element* element) override;

    void Tick(int x, int y, int& frame, Element* element, PowderGrid& grid) override;

    void SafeTick(int x, int y, int& frame, Element* element, PowderGrid& grid) override;
};