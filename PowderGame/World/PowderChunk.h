#pragma once
#include "../Element/ElementList.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>
#include <iostream>

// Custom hash function for sf::Vector2i
struct Vector2iHash {
	std::size_t operator()(const sf::Vector2i& vec) const {
		return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1);
	}
};

class Chunk {


private:
	std::vector<Element> E;
	std::unordered_map<sf::Vector2i, bool, Vector2iHash> Dirtycells;
	std::vector<bool> Tickcells;
};