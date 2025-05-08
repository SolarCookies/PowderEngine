#include "PowderGrid.h"
#include <omp.h>

PowderGrid::PowderGrid() {
    clear.ID = 0; //Can be used to id 35,000 possible 
    clear.Seed = 0; //Random int assigned on Construct
    clear.Density = 0.0; //Used to know if elements need to swap
    clear.HasDensity = false; //Used to know if the element can swap based on Density
    clear.Sleep = 0; //How many frames the element will wait to tick again
    clear.DirtyTick = false; //Runs the normal Tick if true, otherwise uses multithreaded SafeTick
    clear.Color = sf::Color(0, 0, 0, 0); //Same size as a int (4 bytes)
    clear.metadata.empty(); //Used to store custom parameters
}

void PowderGrid::update() {
    Dirtycells.clear();
    if (paused) return;
    frame++;
    
    if (E.size() == 0) { return; }

#pragma omp parallel
    {
#pragma omp for collapse(2)
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                if (Get2D(E, x, y)->ID != 0) {
                    if (get(x, y)->Sleep == 0) {
                        scripts->GetScript(get(x, y)->ID)->SafeTick(x, y, frame, get(x, y), *this);
                        if (get(x, y)->DirtyTick) {
                            Tickcells[GetIndex(x, y)] = true;
                        }
                    }
                    else {
                        get(x, y)->Sleep -= 1;
                    }
                }
            }
        }
    }

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            
            if (Tickcells[GetIndex(x, y)]) {
                scripts->GetScript(get(x, y)->ID)->Tick(x, y, frame, get(x, y), *this);
                Tickcells[GetIndex(x, y)] = false;
            }
            
        }
    }
}

void PowderGrid::draw(sf::RenderTexture& viewport) {
    if (ForceDraw) {
        viewport.clear(sf::Color::Black);

        // Create a vertex array to store triangles
        sf::VertexArray vertices(sf::PrimitiveType::Triangles);
        vertices.resize((GRID_HEIGHT * GRID_WIDTH) * 6); // Reserve space for all vertices

        int i = 0;
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                bool renderBlack = false;
                Element* element = Get2D(E, x, y);
                if (!element) {
                    renderBlack = true;
                }

                sf::Color color;
                if (renderBlack) {
                    color = sf::Color(0, 0, 0, 0);
                }
                else {
                    color = element->Color;
                }

                // Ensure the pixel is written even if alpha is 0
                float x1 = x * CELL_SIZE;
                float y1 = y * CELL_SIZE;
                float x2 = (x + 1) * CELL_SIZE;
                float y2 = (y + 1) * CELL_SIZE;

                // Define the two triangles that form the quad
                // First triangle
                vertices[i].position = sf::Vector2f(x1, y1);
                vertices[i].color = color;

                vertices[i + 1].position = sf::Vector2f(x2, y1);
                vertices[i + 1].color = color;

                vertices[i + 2].position = sf::Vector2f(x1, y2);
                vertices[i + 2].color = color;

                // Second triangle
                vertices[i + 3].position = sf::Vector2f(x2, y1);
                vertices[i + 3].color = color;

                vertices[i + 4].position = sf::Vector2f(x2, y2);
                vertices[i + 4].color = color;

                vertices[i + 5].position = sf::Vector2f(x1, y2);
                vertices[i + 5].color = color;

                i += 6;
            }
        }

        // Resize the vertex array to the actual number of vertices used
        vertices.resize(i);

        // Set a blending mode that forces writing all pixels
        sf::RenderStates states;
        states.blendMode = sf::BlendNone; // Makes sure alpha actually writes to the texture

        // Draw the vertex array
        viewport.draw(vertices, states);
        return;
    }
    if (Dirtycells.empty()) {
        return; // No need to draw if there are no updated cells
    }

    // Create a vertex array to store triangles
    sf::VertexArray vertices(sf::PrimitiveType::Triangles);
    vertices.resize(Dirtycells.size() * 6); // Reserve space for all vertices

    int i = 0;
    for (const auto& [position, elementbool] : Dirtycells) {
        bool renderBlack = false;
        Element* element = Get2D(E, position.x, position.y);
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
            color = element->Color;
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

Element* PowderGrid::get(int x, int y) {
    return Get2D(E, x, y);
}

void PowderGrid::set(int x, int y, Element element) {
    int index = GetIndex(x, y);
    if (index == -1) return;

    if (element.ID == 0) {
        clearElement({ x, y });
        Dirtycells[{x, y}] = true;
    }
    else {
        E[index] = element;
        Dirtycells[{x, y}] = true;
    }
}

void PowderGrid::resize(int width, int height) {
    GRID_HEIGHT = height / CELL_SIZE;
    GRID_WIDTH = width / CELL_SIZE;

    E.clear();
    E.resize(GRID_HEIGHT * GRID_WIDTH);
    Dirtycells.clear();
    Tickcells.clear();
    Tickcells.resize(E.size());
    
	Redraw(); // Force all cells to be updated
}

void PowderGrid::move(int fromX, int fromY, int toX, int toY) {
    if (GetIndex(toX, toY) == -1) return;
    if (GetIndex(fromX, fromY) == -1) return;

    //Move the element and forceably overide what was there
    E[GetIndex(toX, toY)] = E[GetIndex(fromX, fromY)];
    clearElement({ fromX, fromY });
    Dirtycells[sf::Vector2i(toX, toY)] = true;
    Dirtycells[sf::Vector2i(fromX, fromY)] = true;
    
}

void PowderGrid::swap(int x1, int y1, int x2, int y2) {

    if (GetIndex(x1, y1) == -1) return;
    if (GetIndex(x2, y2) == -1) return;

    //Store elements so we can swap safely
    Element it1 = *get(x1,y1);
    Element it2 = *get(x2, y2);

	if (!get(x1, y1) || !get(x2, y2)) {
		return;
    }
    else {
		//Add the elements back to cells but swaped
        E[GetIndex(x1, y1)] = it2;
        E[GetIndex(x2, y2)] = it1;
        Dirtycells[sf::Vector2i(x1, y1)] = true;
        Dirtycells[sf::Vector2i(x2, y2)] = true;
    }

}

void PowderGrid::clearElement(sf::Vector2i Key) {
    if (GetIndex(Key.x, Key.y) == -1) return;
    if (get(Key.x, Key.y)) {
        get(Key.x, Key.y)->ID = clear.ID;
        get(Key.x, Key.y)->Color = clear.Color;
        Dirtycells[Key] = true;
    }
}
