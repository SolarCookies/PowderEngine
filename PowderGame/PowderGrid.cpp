#include "PowderGrid.h"
#include "Element/EmptyElement.h"
#include <omp.h>

PowderGrid::PowderGrid() {
}

void PowderGrid::update() {
    if (paused) {
        return;
    }

    frame++;

    
    // Move all updated cells to the main cells map
    for (auto& [position, element] : updatedcells) {
        if (!element) { continue; }
		if (element->isEmpty()) {
			// If the element is empty, remove it from the cells map
			cells.erase(position);
		}
		else {
			// Otherwise, add it to the cells map
			cells[position] = std::move(element);
		}
    }
    updatedcells.clear();

    // Create a vector of iterators to avoid redundant lookups
    std::vector<std::unordered_map<sf::Vector2i, std::unique_ptr<PowderElement>, Vector2iHash>::iterator> elements;
    elements.reserve(cells.size());

    for (std::unordered_map<sf::Vector2i, std::unique_ptr<PowderElement>, Vector2iHash>::iterator it = cells.begin(); it != cells.end(); ++it) {
        elements.push_back(it);
    }

    // Sort the iterators based on position
    std::sort(elements.begin(), elements.end(), [](const auto& a, const auto& b) {
        const sf::Vector2i& posA = a->first;
        const sf::Vector2i& posB = b->first;
        return (posA.y > posB.y) || (posA.y == posB.y && posA.x < posB.x);
        });

    for (int i = 0; i < static_cast<int>(elements.size()); ++i) {
        std::unique_ptr<PowderElement>& element = elements[i]->second;
        if (!element) continue;
        if (!element->isEmpty()) {
            // Skip if the element is sleeping
            if (element->GetInt("Sleep") > 0) {
                element->SetInt("Sleep", element->GetInt("Sleep") - 1);
                element->Sleep(elements[i]->first.x, elements[i]->first.y, frame);
                continue;
            }
            element->Tick(elements[i]->first.x, elements[i]->first.y, frame);

        }
    }
}

void PowderGrid::draw(sf::RenderTexture& viewport) {
    if (updatedcells.empty()) {
        return; // No need to draw if there are no updated cells
    }

    // Create a vertex array to store triangles
    sf::VertexArray vertices(sf::PrimitiveType::Triangles);
    vertices.resize(updatedcells.size() * 6); // Reserve space for all vertices

    int i = 0;
    for (const auto& [position, element] : updatedcells) {
        if (!element) { continue; }

        if (position.x < 0 || position.y < 0 || position.x >= GRID_WIDTH || position.y >= GRID_HEIGHT)
            continue;

        sf::Color color = element->GetColor("Color");

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
    if (x < 0 || y < 0 || x >= GRID_WIDTH || y >= GRID_HEIGHT) {
        assert(false && "Attempted to access out-of-bounds grid cell");
        return nullptr;
    }

    auto updatedIt = updatedcells.find(sf::Vector2i(x, y));
    if (updatedIt != updatedcells.end() && updatedIt->second) {
        return updatedIt->second.get();
    }
    else {
        auto it = cells.find(sf::Vector2i(x, y));
        if (it != cells.end() && it->second) {
            return it->second.get();
        }
    }

    return nullptr;
}

void PowderGrid::set(int x, int y, std::unique_ptr<PowderElement> element) {
    if (x < 0 || y < 0 || x >= GRID_WIDTH || y >= GRID_HEIGHT) {
        assert(false && "Attempted to access out-of-bounds grid cell");
        return;
    }

    if (element->isEmpty()) {
		//set empty element in updatedcells
		updatedcells[sf::Vector2i(x, y)] = std::make_unique<EmptyElement>();
    }
    else {
        updatedcells[sf::Vector2i(x, y)] = std::move(element);
    }
}

void PowderGrid::resize(int width, int height) {
    GRID_HEIGHT = height / 4;
    GRID_WIDTH = width / 4;

    for (auto it = cells.begin(); it != cells.end();) {
        const sf::Vector2i& position = it->first;

        if (position.x >= GRID_WIDTH || position.y >= GRID_HEIGHT) {
            it = cells.erase(it);
        }
        else {
            ++it;
        }
    }
	for (auto it = updatedcells.begin(); it != updatedcells.end();) {
		const sf::Vector2i& position = it->first;
		if (position.x >= GRID_WIDTH || position.y >= GRID_HEIGHT) {
			it = updatedcells.erase(it);
		}
		else {
			++it;
		}
	}
	Redraw(); // Force all cells to be updated
}


void PowderGrid::move(int fromX, int fromY, int toX, int toY) {
    if (fromX < 0 || fromY < 0 || toX < 0 || toY < 0 ||
        fromX >= GRID_WIDTH || fromY >= GRID_HEIGHT ||
        toX >= GRID_WIDTH || toY >= GRID_HEIGHT) {
        assert(false && "Attempted to move out-of-bounds grid cell");
        return;
    }

    // Find the element at the source position
    auto it = cells.find(sf::Vector2i(fromX, fromY));
    if (it != cells.end()) {
        // Move the element to the destination position
        updatedcells[sf::Vector2i(toX, toY)] = std::move(it->second);
        cells.erase(it);

        // Set the previous position to an empty element
        updatedcells[sf::Vector2i(fromX, fromY)] = std::make_unique<EmptyElement>();
    }
}

void PowderGrid::swap(int x1, int y1, int x2, int y2) {
	// Will swap the elements at (x1, y1) and (x2, y2) and place them both in the updatedcells map
	if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 ||
		x1 >= GRID_WIDTH || y1 >= GRID_HEIGHT ||
		x2 >= GRID_WIDTH || y2 >= GRID_HEIGHT) {
		assert(false && "Attempted to swap out-of-bounds grid cell");
		return;
	}
	auto it1 = cells.find(sf::Vector2i(x1, y1));
	auto it2 = cells.find(sf::Vector2i(x2, y2));
	if (!it1->second || !it2->second) {
		return;
    }
    else {
		//Add the elements to the updatedcells map
		updatedcells[sf::Vector2i(x1, y1)] = std::move(it2->second);
		updatedcells[sf::Vector2i(x2, y2)] = std::move(it1->second);
    }

}

void PowderGrid::clearElement(sf::Vector2i Key) {
	// add empty element to updatedcells
	updatedcells[Key] = std::make_unique<EmptyElement>();
}
