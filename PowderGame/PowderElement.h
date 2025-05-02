#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <random>
#include <limits.h>

const int CELL_SIZE = 4;  // Each element is a 4x4 square
inline int GRID_WIDTH = 128;
inline int GRID_HEIGHT = 128;


class PowderElement {
public:
    virtual ~PowderElement() = default;

    virtual void update(int x, int y, class PowderGrid& grid) = 0;
	virtual void updateHeat(int x, int y, class PowderGrid& grid) { return; }
	virtual std::unique_ptr<PowderElement> clone() const = 0;
    virtual sf::Color getColor() = 0;
	virtual bool isEmpty() const { return false; };
	virtual int getDensity() { return 1; };
	virtual bool isSolid() const { return false; } // Used to identify if the element is solid or not (for example, sand is not solid, but concrete is solid)

	//Used to identify the element without casting or having a direct reference to the class
	virtual std::string GetName() const { return "None"; }

	//pcghash
	static uint32_t pcg_hash(uint32_t input){
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	static float getRandomFloat(uint32_t& seed) {
		seed = pcg_hash(seed);
		return static_cast<float>(seed) / static_cast<float>(UINT32_MAX);
	}

	static int getRandomInt(uint32_t& seed, int min, int max) {
		seed = pcg_hash(seed);
		return (seed % (max - min + 1)) + min;
	}


	int tickCounter = 0; // Tracks ticks for sleep mode (If 0 then the element is active, if > 0 then the element is sleeping for a number of ticks)
	float temprature = 0; // Start temperature of the element
	float energy = 0; // Start energy of the element
	uint32_t seed; // Seed for random number generation
	int randomDirection = 0; // Random direction for movement
	int id; // ID for the element

};