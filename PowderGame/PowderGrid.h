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

	inline int GetIndex(int x, int y, bool wrap = false) {
		if (wrap) {
			x = (x % GRID_WIDTH + GRID_WIDTH) % GRID_WIDTH;
			y = (y % GRID_HEIGHT + GRID_HEIGHT) % GRID_HEIGHT;
		}
		else {
			if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
				return -1; // Invalid index
			}
		}

		return y * GRID_WIDTH + x;
	}

	PowderElement* Get2D(std::vector<std::unique_ptr<PowderElement>>& vec, int x, int y, bool wrap = false) {
		if (wrap) {
			x = (x % GRID_WIDTH + GRID_WIDTH) % GRID_WIDTH;
			y = (y % GRID_HEIGHT + GRID_HEIGHT) % GRID_HEIGHT;
		}
		else {
			if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
				return nullptr;
			}
		}

		int index = y * GRID_WIDTH + x;

		// Correct bounds check
		if (index < 0 || index >= static_cast<int>(vec.size())) {
			return nullptr;
		}

		return vec[index].get(); // Safe now
	}

    void update();

	void draw(sf::RenderTexture& viewport);

    PowderElement* get(int x, int y);

    void set(int x, int y, std::unique_ptr<PowderElement> element);

	void move(int fromX, int fromY, int toX, int toY);

	void swap(int x1, int y1, int x2, int y2);

    void resize(int width, int height);

	void Redraw() {
		
	}

	void clearElement(sf::Vector2i Key);

	sf::Vector2i getSize() const {
		return sf::Vector2i(GRID_WIDTH, GRID_HEIGHT);
	}

	int getCount() const {
		return num;
	}

	bool isEmpty(int x, int y) {
		if (Get2D(cells,x,y)) {
			return false;
		}
		else {
			return true;
		}
	}

	int frame;
	bool paused = false;
	MonoDomain* domain = nullptr;

private:
	// Use the custom hash function for the unordered_map
	std::vector<std::unique_ptr<PowderElement>> cells;
	std::unordered_map<sf::Vector2i, bool, Vector2iHash> Dirtycells;
	std::vector<bool> Tickcells;
	int num;
};