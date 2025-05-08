#pragma once
#include "PowderChunk.h"

class PowderGrid {
public:
	const int CELL_SIZE = 4;  // Each element is a 4x4 square
	int GRID_WIDTH = 128;
	int GRID_HEIGHT = 128;

    PowderGrid();

	inline int GetIndex(int x, int y) {
		if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
			return -1; // Invalid index
		}

		return y * GRID_WIDTH + x;
	}

	Element* Get2D(std::vector<Element>& vec, int x, int y) {

		int index = GetIndex(x, y);
		if (index == -1) return nullptr;

		// Correct bounds check
		if (index < 0 || index >= static_cast<int>(vec.size())) {
			return nullptr;
		}

		return &vec[index];
	}

    void update();

	void draw(sf::RenderTexture& viewport);

	//Getsjkfdjak f
	Element* get(int x, int y);

    void set(int x, int y, Element element);

	void move(int fromX, int fromY, int toX, int toY);

	void swap(int x1, int y1, int x2, int y2);

    void resize(int width, int height);

	void Redraw() {
		
	}

	void clearElement(sf::Vector2i Key);

	sf::Vector2i getSize() const {
		return sf::Vector2i(GRID_WIDTH, GRID_HEIGHT);
	}

	int getCount() {
		num = 0;
		for (int i = 0; i <= E.size(); i++) {
			if (E[i].ID != 0) {
				num++;
			}
		}
		return num;
	}

	bool isEmpty(int x, int y) {
		if (GetIndex(x, y) == -1) return true;
		if (Get2D(E,x,y)->ID != 0) {
			return false;
		}
		else {
			return true;
		}
	}
	void MarkDirty(int x, int y) {
		Dirtycells[{x, y}] = true;
	}

	int frame;
	bool paused = false;
	elements* scripts;

private:

	std::vector<Element> E;
	std::unordered_map<sf::Vector2i, bool, Vector2iHash> Dirtycells;
	std::vector<bool> Tickcells;
	int num;
	bool ForceDraw = false;
	Element clear;
};