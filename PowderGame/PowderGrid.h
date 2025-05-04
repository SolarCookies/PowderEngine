#pragma once
#include "Elements/PowderElement.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>

#include <SFML/Graphics.hpp>

struct RawPowderElementParameters {
	int id;
	std::string name;
	bool isSolid;
	int density;
};

// Custom hash function for sf::Vector2i
struct Vector2iHash {
    std::size_t operator()(const sf::Vector2i& vec) const {
        return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1);
    }
};

class PowderGrid {
public:
    PowderGrid();

    void update();
    void draw(sf::RenderWindow& window);
	void draw(sf::RenderTexture& viewport);
    PowderElement* get(int x, int y);
    void set(int x, int y, std::unique_ptr<PowderElement> element);
	void setheat(int x, int y, float heat) {
	}

	void move(int fromX, int fromY, int toX, int toY);

	void swap(int x1, int y1, int x2, int y2);

	//Attemps to resize the grid only removes the elements that are out of bounds
    void resize(int width, int height);

	void Redraw() {
		//set updatedcells to cells (Force all cells to be updated)
		for (auto it = cells.begin(); it != cells.end(); ++it) {
			updatedcells[it->first] = std::move(it->second);
		}
	}

	void clearElement(sf::Vector2i Key);

	sf::Vector2i getSize() const {
		return sf::Vector2i(GRID_WIDTH, GRID_HEIGHT);
	}

	int getCount() const {
		return cells.size();
	}

	RawPowderElementParameters getRaw(int x, int y) {
		PowderElement* element = get(x, y);
		if (element) {
			return { element->id, element->GetName(), element->isSolid(), element->getDensity() };
		}
		else {
			return { -1, "None", false, 0 };
		}
	}

	int getID(int x, int y) {
		PowderElement* element = get(x, y);
		if (element) {
			if (element->isEmpty()) {
				return 0;
			}
			return element->id;
		}
		else {
			return 0;
		}
	}

	int getDensity(int x, int y) {
		PowderElement* element = get(x, y);
		if (element) {
			return element->getDensity();
		}
		else {
			return 0;
		}
	}

	int getTemperature(int x, int y) {
		PowderElement* element = get(x, y);
		if (element) {
			return element->temprature;
		}
		else {
			return 0;
		}
	}

	int getEnergy(int x, int y) {
		PowderElement* element = get(x, y);
		if (element) {
			return element->energy;
		}
		else {
			return 0;
		}
	}

	std::string getName(int x, int y) {
		PowderElement* element = get(x, y);
		if (element) {
			return element->GetName();
		}
		else {
			return "None";
		}
	}

	bool isSolid(int x, int y) {
		PowderElement* element = get(x, y);
		if (element) {
			return element->isSolid();
		}
		else {
			return false;
		}
	}

	bool isEmpty(int x, int y) {
		PowderElement* element = get(x, y);
		if (element) {
			return element->isEmpty();
		}
		else {
			return true;
		}
	}



	int frame;


private:
	// Use the custom hash function for the unordered_map
	std::unordered_map<sf::Vector2i, std::unique_ptr<PowderElement>, Vector2iHash> cells;
	std::unordered_map<sf::Vector2i, std::unique_ptr<PowderElement>, Vector2iHash> updatedcells;

    
};