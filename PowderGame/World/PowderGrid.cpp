// PowderGrid.cpp
#include "PowderGrid.h"
#include <omp.h>

PowderGrid::PowderGrid() {
	clear.ID = 0;
	clear.Seed = 0;
	clear.Density = 0.0;
	clear.HasDensity = false;
	clear.Sleep = 0;
	clear.DirtyTick = false;
	clear.Color = sf::Color(0, 0, 0, 0);
	chunks.resize(GRID_WIDTH * GRID_HEIGHT);
	for (auto& chunk : chunks) {
		chunk = std::make_unique<Chunk>();
	}
}

Element* PowderGrid::get(int x, int y) {
	sf::Vector2i chunkPos = WorldToChunkCoord(x, y);
	sf::Vector2i localPos = LocalToChunkCoord(x, y);
	int index = chunkPos.y * GRID_WIDTH + chunkPos.x;
	if (index < 0 || index >= static_cast<int>(chunks.size())) return nullptr;
	return chunks[index]->get(localPos.x, localPos.y);
}

void PowderGrid::set(int x, int y, Element element) {
	sf::Vector2i chunkPos = WorldToChunkCoord(x, y);
	sf::Vector2i localPos = LocalToChunkCoord(x, y);
	int index = chunkPos.y * GRID_WIDTH + chunkPos.x;
	if (index < 0 || index >= static_cast<int>(chunks.size())) return;
	chunks[index]->set(localPos.x, localPos.y, element);
}

void PowderGrid::update() {
	if (paused) return;
	frame++;

#pragma omp parallel for collapse(2)
	for (int cy = 0; cy < GRID_HEIGHT; ++cy) {
		for (int cx = 0; cx < GRID_WIDTH; ++cx) {
			int index = cy * GRID_WIDTH + cx;
			if (!chunks[index].get()) continue;
			for (int y = 0; y < Chunk::ChunkSize; ++y) {
				for (int x = 0; x < Chunk::ChunkSize; ++x) {
					if (chunks[index].get()->get(x, y) && chunks[index].get()->get(x, y)->ID != 0) {
						if (chunks[index].get()->get(x, y)->Sleep == 0) {
							scripts->GetScript(chunks[index].get()->get(x, y)->ID)->SafeTick((cx * Chunk::ChunkSize + x), (cy * Chunk::ChunkSize + y), frame, chunks[index].get()->get(x, y), *this);
							if (chunks[index].get()->get(x, y)->DirtyTick) {
								chunks[index].get()->Tickcells[chunks[index].get()->GetIndex(x, y)] = true;
							}
						}
						else {
							chunks[index].get()->get(x, y)->Sleep--;
						}
					}
				}
			}
		}
	}

	for (int cy = 0; cy < GRID_HEIGHT; ++cy) {
		for (int cx = 0; cx < GRID_WIDTH; ++cx) {
			int index = cy * GRID_WIDTH + cx;
			if (!chunks[index].get()) continue;
			for (int y = 0; y < Chunk::ChunkSize; ++y) {
				for (int x = 0; x < Chunk::ChunkSize; ++x) {
					if (chunks[index].get()->Tickcells[chunks[index].get()->GetIndex(x, y)]) {
						scripts->GetScript(get((cx * Chunk::ChunkSize + x), (cy * Chunk::ChunkSize + y))->ID)->Tick((cx * Chunk::ChunkSize + x), (cy * Chunk::ChunkSize + y), frame, get((cx * Chunk::ChunkSize + x), (cy * Chunk::ChunkSize + y)), *this);
						chunks[index].get()->Tickcells[chunks[index].get()->GetIndex(x, y)] = false;
					}
				}
			}
		}
	}
}

void PowderGrid::draw(sf::RenderTexture& viewport) {
	viewport.clear(sf::Color::Black);
	sf::VertexArray vertices(sf::PrimitiveType::Triangles);

	// Draw particle triangles
	for (int cy = 0; cy < GRID_HEIGHT; ++cy) {
		for (int cx = 0; cx < GRID_WIDTH; ++cx) {
			int index = cy * GRID_WIDTH + cx;
			Chunk* chunk = chunks[index].get();
			if (!chunk) continue;
			for (const auto& dirty : chunk->Dirtycells) {
				sf::Vector2i localPos = dirty.first;
				Element* element = chunk->get(localPos.x, localPos.y);
				if (!element) continue;

				int worldX = cx * Chunk::ChunkSize + localPos.x;
				int worldY = cy * Chunk::ChunkSize + localPos.y;

				float x = worldX * CELL_SIZE;
				float y = worldY * CELL_SIZE;
				float x1 = x + CELL_SIZE;
				float y1 = y + CELL_SIZE;
				sf::Color color = element->Color;

				vertices.append({ {x, y}, color });
				vertices.append({ {x1, y}, color });
				vertices.append({ {x, y1}, color });
				vertices.append({ {x1, y}, color });
				vertices.append({ {x1, y1}, color });
				vertices.append({ {x, y1}, color });
			}
			chunk->Dirtycells.clear();
		}
	}

	sf::RenderStates states;
	states.blendMode = sf::BlendNone;
	viewport.draw(vertices, states);

	// Draw chunk outlines using drawLine
	sf::Color lineColor(50, 50, 50, 255);
	int lineWidth = 1;

	// Horizontal lines
	for (int cy = 0; cy <= GRID_HEIGHT; ++cy) {
		int y = cy * Chunk::ChunkSize * CELL_SIZE;
		sf::Vector2i p1(0, y);
		sf::Vector2i p2(GRID_WIDTH * Chunk::ChunkSize * CELL_SIZE, y);
		drawLine(viewport, p1, p2, lineWidth, lineColor);
	}

	// Vertical lines
	for (int cx = 0; cx <= GRID_WIDTH; ++cx) {
		int x = cx * Chunk::ChunkSize * CELL_SIZE;
		sf::Vector2i p1(x, 0);
		sf::Vector2i p2(x, GRID_HEIGHT * Chunk::ChunkSize * CELL_SIZE);
		drawLine(viewport, p1, p2, lineWidth, lineColor);
	}
}

void PowderGrid::resize(int width, int height) {
	GRID_HEIGHT = height / CELL_SIZE;
	GRID_WIDTH = width / CELL_SIZE;
	chunks.clear();
	chunks.resize(GRID_WIDTH * GRID_HEIGHT);
	for (auto& chunk : chunks) {
		chunk = std::make_unique<Chunk>();
	}
	Redraw();
}

void PowderGrid::clearElement(sf::Vector2i Key) {
	set(Key.x, Key.y, clear);
}

bool PowderGrid::isEmpty(int x, int y) {
	Element* e = get(x, y);
	return (!e || e->ID == 0);
}

void PowderGrid::MarkDirty(int x, int y) {
	sf::Vector2i chunkPos = WorldToChunkCoord(x, y);
	sf::Vector2i localPos = LocalToChunkCoord(x, y);
	int index = chunkPos.y * GRID_WIDTH + chunkPos.x;
	if (index < 0 || index >= static_cast<int>(chunks.size())) return;
	chunks[index]->Dirtycells[localPos] = true;
}

int PowderGrid::getCount() {
	int count = 0;
	for (auto& chunk : chunks) {
		if (!chunk) continue;
		for (Element& e : chunk->E) {
			if (e.ID != 0) count++;
		}
	}
	return count;
}

void PowderGrid::Redraw() {
	for (auto& chunk : chunks) {
		if (!chunk) continue;
		for (int y = 0; y < Chunk::ChunkSize; ++y) {
			for (int x = 0; x < Chunk::ChunkSize; ++x) {
				chunk->Dirtycells[{x, y}] = true;
			}
		}
	}
}

void PowderGrid::move(int fromX, int fromY, int toX, int toY) {
	Element* from = get(fromX, fromY);
	if (!from) return;

	set(toX, toY, *from);
	clearElement({ fromX, fromY });
}

void PowderGrid::swap(int x1, int y1, int x2, int y2) {
	Element* a = get(x1, y1);
	Element* b = get(x2, y2);
	if (!a || !b) return;

	Element temp = *a;
	set(x1, y1, *b);
	set(x2, y2, temp);
}
