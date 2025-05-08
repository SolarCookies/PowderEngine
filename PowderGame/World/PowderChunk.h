// PowderChunk.h
#pragma once
#include "../Element/ElementList.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>

struct Vector2iHash {
	std::size_t operator()(const sf::Vector2i& vec) const {
		return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1);
	}
};

class Chunk {
public:
	static constexpr int ChunkSize = 64;

	Chunk() {
		E.resize(ChunkSize * ChunkSize);
		Tickcells.resize(ChunkSize * ChunkSize);
	}

	inline int GetIndex(int x, int y) const {
		if (x < 0 || x >= ChunkSize || y < 0 || y >= ChunkSize) return -1;
		return y * ChunkSize + x;
	}

	Element* get(int x, int y) {
		int index = GetIndex(x, y);
		if (index == -1) return nullptr;
		return &E[index];
	}

	void set(int x, int y, Element element) {
		int index = GetIndex(x, y);
		if (index == -1) return;
		E[index] = element;
		Dirtycells[{x, y}] = true;
	}

	std::vector<Element> E;
	std::unordered_map<sf::Vector2i, bool, Vector2iHash> Dirtycells;
	std::vector<bool> Tickcells;
};
