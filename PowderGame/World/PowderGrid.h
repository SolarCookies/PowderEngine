// PowderGrid.h
#pragma once
#include "PowderChunk.h"

class PowderGrid {
public:
	const int CELL_SIZE = 4;
	int GRID_WIDTH = 4;
	int GRID_HEIGHT = 4;

	PowderGrid();

	Element* get(int x, int y);
	void set(int x, int y, Element element);
	void move(int fromX, int fromY, int toX, int toY);
	void swap(int x1, int y1, int x2, int y2);
	void update();
	void draw(sf::RenderTexture& viewport);
	void resize(int width, int height);
	void clearElement(sf::Vector2i Key);
	bool isEmpty(int x, int y);
	void MarkDirty(int x, int y);
	void Redraw();
	int getCount();

	sf::Vector2i getSize() const {
		return sf::Vector2i(GRID_WIDTH, GRID_HEIGHT);
	}

	sf::Vector2i WorldToChunkCoord(int x, int y) {
		return { x / Chunk::ChunkSize, y / Chunk::ChunkSize };
	}

	sf::Vector2i LocalToChunkCoord(int x, int y) {
		int lx = x % Chunk::ChunkSize;
		int ly = y % Chunk::ChunkSize;
		if (lx < 0) lx += Chunk::ChunkSize;
		if (ly < 0) ly += Chunk::ChunkSize;
		return { lx, ly };
	}

    void drawLine(sf::RenderTexture& viewport, sf::Vector2i point1, sf::Vector2i point2, int lineWidth, sf::Color lineColor)
    {
        int x0 = point1.x;
        int y0 = point1.y;
        int x1 = point2.x;
        int y1 = point2.y;
        int dx = abs(x1 - x0);
        int sx, sy;
        if (x0 < x1) {
            sx = 1;
        }
        else {
            sx = -1;
        }

        int dy = -abs(y1 - y0);
        if (y0 < y1) {
            sy = 1;
        }
        else {
            sy = -1;
        }

        int err = dx + dy;

        while (true) {
            sf::RectangleShape rect(sf::Vector2f(lineWidth, lineWidth));
            rect.setFillColor(lineColor);
            rect.setPosition({ static_cast<float>(x0), static_cast<float>(y0) });
            viewport.draw(rect);
            if (x0 == x1 && y0 == y1) {
                break;
            }
            int e2 = 2 * err;

            if (e2 >= dy) {
                err += dy;
                x0 += sx;
            }

            if (e2 <= dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

	int frame;
	bool paused = false;
	elements* scripts;

private:
	std::vector<std::unique_ptr<Chunk>> chunks;
	Element clear;
};