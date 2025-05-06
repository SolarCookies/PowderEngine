#pragma once
#include "Element/PowderElement.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>
#include <iostream>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>

#include <SFML/Graphics.hpp>

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

	void draw(sf::RenderTexture& viewport);

    PowderElement* get(int x, int y);

    void set(int x, int y, std::unique_ptr<PowderElement> element);

	void move(int fromX, int fromY, int toX, int toY);

	void swap(int x1, int y1, int x2, int y2);

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

	bool isEmpty(int x, int y) {
		if (cells[{x, y}] || updatedcells[{x, y}]) {
			return false;
		}
		else {
			return true;
		}
	}

	int frame;
	bool paused = false;


private:
	// Use the custom hash function for the unordered_map
	std::unordered_map<sf::Vector2i, std::unique_ptr<PowderElement>, Vector2iHash> cells;
	std::unordered_map<sf::Vector2i, std::unique_ptr<PowderElement>, Vector2iHash> updatedcells;

    
};