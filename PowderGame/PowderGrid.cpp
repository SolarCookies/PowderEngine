#include "PowderGrid.h"
#include "Element/EmptyElement.h"
#include "mono/jit/jit.h"
#include "mono/metadata/threads.h"
#include <omp.h>

PowderGrid::PowderGrid() {
}

void PowderGrid::update() {
    Dirtycells.clear();
    if (paused) return;
    frame++;
    
    if (cells.size() == 0) { return; }

#pragma omp parallel
    {
        thread_local MonoThread* monoThread = nullptr;

        if (!monoThread) {
            monoThread = mono_thread_attach(domain);
        }

#pragma omp for collapse(2)
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                if (Get2D(cells, x, y)) {
                    if (Get2D(cells, x, y)->GetInt("Sleep") == 0) {
                        Get2D(cells, x, y)->SafeTick(x, y, frame);
                        if (Get2D(cells, x, y)->IsDirty()) {
                            Tickcells[GetIndex(x, y)] = true;
                        }
                    }
                    else {
                        Get2D(cells, x, y)->SetInt("Sleep", (Get2D(cells, x, y)->GetInt("Sleep") - 1));
                    }
                }
            }
        }
    }

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            
            if (Tickcells[GetIndex(x, y)]) {
                Get2D(cells, x, y)->Tick(x, y, frame);
                Tickcells[GetIndex(x, y)] = false;
            }
            
        }
    }
}

void PowderGrid::draw(sf::RenderTexture& viewport) {
    if (Dirtycells.empty()) {
        return; // No need to draw if there are no updated cells
    }

    // Create a vertex array to store triangles
    sf::VertexArray vertices(sf::PrimitiveType::Triangles);
    vertices.resize(Dirtycells.size() * 6); // Reserve space for all vertices

    int i = 0;
    for (const auto& [position, elementbool] : Dirtycells) {
        bool renderBlack = false;
        PowderElement* element = Get2D(cells, position.x, position.y);
        if (!element) {
            clearElement(position);
            renderBlack = true;
        }

        if (position.x < 0 || position.y < 0 || position.x >= GRID_WIDTH || position.y >= GRID_HEIGHT)
            continue;

        sf::Color color;
        if (renderBlack) {
            color = sf::Color(0, 0, 0, 0);
        }
        else {
            color = element->GetColor("Color");
        }

        // Ensure the pixel is written even if alpha is 0
        float x = position.x * CELL_SIZE;
        float y = position.y * CELL_SIZE;
        float x1 = x + CELL_SIZE;
        float y1 = y + CELL_SIZE;

        // Define the two triangles that form the quad
        // First triangle
        vertices[i].position = sf::Vector2f(x, y);
        vertices[i].color = color;

        vertices[i + 1].position = sf::Vector2f(x1, y);
        vertices[i + 1].color = color;

        vertices[i + 2].position = sf::Vector2f(x, y1);
        vertices[i + 2].color = color;

        // Second triangle
        vertices[i + 3].position = sf::Vector2f(x1, y);
        vertices[i + 3].color = color;

        vertices[i + 4].position = sf::Vector2f(x1, y1);
        vertices[i + 4].color = color;

        vertices[i + 5].position = sf::Vector2f(x, y1);
        vertices[i + 5].color = color;

        i += 6;
    }

    // Resize the vertex array to the actual number of vertices used
    vertices.resize(i);

    // Set a blending mode that forces writing all pixels
    sf::RenderStates states;
	states.blendMode = sf::BlendNone; // Makes sure alpha actually writes to the texture

    // Draw the vertex array
    viewport.draw(vertices, states);

}

PowderElement* PowderGrid::get(int x, int y) {
    return Get2D(cells, x, y);
}

void PowderGrid::set(int x, int y, std::unique_ptr<PowderElement> element) {
    if (element->isEmpty()) {
        clearElement({ x,y });
        Dirtycells[{x, y}] = true;
    }
    else {
        cells[GetIndex(x,y)] = std::move(element);
        Dirtycells[{x, y}] = true;
        num++;
    }
}

void PowderGrid::resize(int width, int height) {
    GRID_HEIGHT = height / 4;
    GRID_WIDTH = width / 4;

    num = 0;
    cells.clear();
    cells.resize(GRID_HEIGHT * GRID_WIDTH);
    Dirtycells.clear();
    Tickcells.clear();
    Tickcells.resize(cells.size());
    
	Redraw(); // Force all cells to be updated
}

void PowderGrid::move(int fromX, int fromY, int toX, int toY) {
    if (fromX < 0 || fromY < 0 || toX < 0 || toY < 0 ||
        fromX >= GRID_WIDTH || fromY >= GRID_HEIGHT ||
        toX >= GRID_WIDTH || toY >= GRID_HEIGHT) {
        return;
    }

    //Move the element and forceably overide what was there
    cells[GetIndex(toX, toY)] = std::move(cells[GetIndex(fromX, fromY)]);
    cells[GetIndex(fromX, fromY)] = nullptr;
    Dirtycells[sf::Vector2i(toX, toY)] = true;
    Dirtycells[sf::Vector2i(fromX, fromY)] = true;
    
}

void PowderGrid::swap(int x1, int y1, int x2, int y2) {
    if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 ||
        x1 >= GRID_WIDTH || y1 >= GRID_HEIGHT ||
        x2 >= GRID_WIDTH || y2 >= GRID_HEIGHT) {
        return;
    }

    //Store elements so we can swap safely
    std::unique_ptr<PowderElement> it1 = std::move(cells[GetIndex(x1, y1)]);
    std::unique_ptr<PowderElement> it2 = std::move(cells[GetIndex(x2, y2)]);

	if (!it1 || !it2) {
		return;
    }
    else {
		//Add the elements back to cells but swaped
        cells[GetIndex(x1, y1)] = std::move(it2);
        cells[GetIndex(x2, y2)] = std::move(it1);
        Dirtycells[sf::Vector2i(x1, y1)] = true;
        Dirtycells[sf::Vector2i(x2, y2)] = true;
    }

}

void PowderGrid::clearElement(sf::Vector2i Key) {
    //set cell at location to nullptr
    cells[GetIndex(Key.x, Key.y)] = nullptr;
    Dirtycells[Key] = true;
    num--;
}
