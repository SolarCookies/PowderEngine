#include "PowderGrid.h"
#include "EmptyElement.h"
#include <omp.h>

PowderGrid::PowderGrid() {
}

void PowderGrid::update() {
    frame++;
    // Create a vector of iterators to avoid redundant lookups
    std::vector<std::unordered_map<sf::Vector2i, std::unique_ptr<PowderElement>, Vector2iHash>::iterator> elements;
    elements.reserve(cells.size());

//#pragma omp parallel for private(elements)
    for (std::unordered_map<sf::Vector2i, std::unique_ptr<PowderElement>, Vector2iHash>::iterator it = cells.begin(); it != cells.end(); ++it) {
        elements.push_back(it);
    }

    // Sort the iterators based on position
    std::sort(elements.begin(), elements.end(), [](const auto& a, const auto& b) {
        const sf::Vector2i& posA = a->first;
        const sf::Vector2i& posB = b->first;
        return (posA.y > posB.y) || (posA.y == posB.y && posA.x < posB.x);
        });

    // Parallelize the update process
//#pragma omp parallel for
    for (int i = 0; i < static_cast<int>(elements.size()); ++i) {
        std::unique_ptr<PowderElement>& element = elements[i]->second;

        if (!element->isEmpty()) {
            // Skip if the element is sleeping
            if (element->tickCounter > 0) {
//#pragma omp critical
                element->tickCounter--;
                continue;
            }

            // Update heat and element behavior
            //element->updateHeat(elements[i]->first.x, elements[i]->first.y, *this);
            element->update(elements[i]->first.x, elements[i]->first.y, *this);
        }
    }
}

void PowderGrid::draw(sf::RenderWindow& window) {
    sf::RectangleShape pixel(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    for (const auto& [position, element] : cells) {
        pixel.setPosition(sf::Vector2f(position.x * CELL_SIZE, position.y * CELL_SIZE));
        pixel.setFillColor(element->getColor());
        window.draw(pixel);
    }
}

void PowderGrid::draw(sf::RenderTexture& viewport) {
    if (cells.empty()) {
        return; // No need to draw if there are no cells
    }

    // Create a vertex array to store triangles
    sf::VertexArray vertices(sf::PrimitiveType::Triangles);
    vertices.resize(cells.size() * 6); // Reserve space for all vertices

    int i = 0;
    for (const auto& [position, element] : cells) {
        sf::Color color = element->getColor();
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

    // Draw the vertex array
    viewport.draw(vertices);
}

PowderElement* PowderGrid::get(int x, int y) {
    if (x < 0 || y < 0 || x >= GRID_WIDTH || y >= GRID_HEIGHT) {
        assert(false && "Attempted to access out-of-bounds grid cell");
        return nullptr;
    }

    auto it = cells.find(sf::Vector2i(x, y));
    if (it != cells.end()) {
        return it->second.get();
    }

    static EmptyElement emptyElement;
    return &emptyElement;
}

void PowderGrid::set(int x, int y, std::unique_ptr<PowderElement> element) {
    if (x < 0 || y < 0 || x >= GRID_WIDTH || y >= GRID_HEIGHT) {
        assert(false && "Attempted to access out-of-bounds grid cell");
        return;
    }

    if (element->isEmpty()) {
        cells.erase(sf::Vector2i(x, y));
    }
    else {
        cells[sf::Vector2i(x, y)] = std::move(element);
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
}