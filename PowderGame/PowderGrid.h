#pragma once
#include "PowderElement.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>

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

	void clearElement(sf::Vector2i Key);

	sf::Vector2i getSize() const {
		return sf::Vector2i(GRID_WIDTH, GRID_HEIGHT);
	}

	int getCount() const {
		return cells.size();
	}

	int frame;

private:
    // Use the custom hash function for the unordered_map
    std::unordered_map<sf::Vector2i, std::unique_ptr<PowderElement>, Vector2iHash> cells;
	std::unordered_map<sf::Vector2i, std::unique_ptr<PowderElement>, Vector2iHash> updatedcells;

    
};