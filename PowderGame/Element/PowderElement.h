#pragma once
#include "SFML/Graphics.hpp"
#include <array>
#include <cstdint>
#include <limits>

class Rand {
public:
    static uint32_t PcgHash(int input) {
        uint32_t x = static_cast<uint32_t>(input);
        x ^= x >> 16;
        x *= 0x85ebca6b;
        x ^= x >> 13;
        x *= 0xc2b2ae35;
        x ^= x >> 16;
        return x;
    }

    static float Float(int seed) {
        uint32_t hashed = PcgHash(seed);
        return static_cast<float>(hashed) / static_cast<float>(std::numeric_limits<int>::max());
    }

    static int Int(int seed, int min, int max) {
        uint32_t hashed = PcgHash(seed);
        int range = max - min + 1;
        return static_cast<int>(hashed % static_cast<uint32_t>(range)) + min;
    }
};

class PowderGrid;

struct Element {
	short ID = 0; //Can be used to id 35,000 possible 
	short Seed = 0; //Random int assigned on Construct
    float Density = 0.0; //Used to know if elements need to swap
    bool HasDensity = false; //Used to know if the element can swap based on Density
	short Sleep = 0; //How many frames the element will wait to tick again
	bool DirtyTick = false; //Runs the normal Tick if true, otherwise uses multithreaded SafeTick
	sf::Color Color = sf::Color(0, 0, 0, 0); //Same size as a int (4 bytes)
	std::array<std::byte, 16> metadata; //Used to store custom parameters
};


class PowderScript {
public:
	short ID; //Used to find what script to run for the element (Should be unique per script)
	std::string Name = "NULL";
	sf::Color initColor = sf::Color(0, 0, 0, 0);
	virtual void OnConstruct(Element* Element) {} //Runs when element is created
	virtual void BeginPlay(int x, int y, int& frame, Element* Element) {} //When placed in world
	virtual void Tick(int x, int y, int& frame, Element* Element, PowderGrid& grid) {} //Runs every frame
	virtual void SafeTick(int x, int y, int& frame, Element* Element, PowderGrid& grid) {} //Runs every frame but cant access Fuctions like move or swap


protected:
	//Common Functions like movement algorithms or common helper Functions
    int Clamp(int value, int min, int max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
};