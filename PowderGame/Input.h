#pragma once
#include "Elements/ElementList.h"
#include "PowderGrid.h"
#include "Window.h"
#include "TextEditor.h"

class Input {
public:
	Input() {}
	~Input() {}

	void TickInput(Window& window, PowderGrid& grid, TextEditor& TE) {
        // Process events
        while (const auto event = window.getWindowHandle().pollEvent()) {
            ImGui::SFML::ProcessEvent(window.getWindowHandle(), *event);

            if (event->is<sf::Event::Closed>()) {
                window.getWindowHandle().close();
            }
        }

        // Handle mouse button events
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            // Get mouse position
            sf::Vector2i mousePos = sf::Mouse::getPosition(window.getWindowHandle());
            // Convert to grid coordinates
            int gridX = mousePos.x / 4; // Assuming each cell is 10x10 pixels
            int gridY = mousePos.y / 4;

            // Apply brush radius
            for (int dx = -BrushRadius; dx <= BrushRadius; ++dx)
            {
                for (int dy = -BrushRadius; dy <= BrushRadius; ++dy)
                {
                    if (dx * dx + dy * dy <= BrushRadius * BrushRadius) // Circle check
                    {
                        // Call grid.set() with the current element
                        if (grid.get(gridX + dx, gridY + dy) == nullptr || grid.get(gridX + dx, gridY + dy)->isEmpty()) {
                            //random int using <random>
                            std::random_device rd;  // Obtain a random number from hardware
                            std::mt19937 gen(rd()); // Seed the generator
                            int rand = std::uniform_int_distribution<>(0, 1000000)(gen); // Random int between 0 and 1000000
                            grid.set(gridX + dx, gridY + dy, currentElement->clone());
                            if (grid.get(gridX + dx, gridY + dy) != nullptr) {
                                grid.get(gridX + dx, gridY + dy)->id = rand; // Set the ID of the element
                            }
                        }

                    }
                }
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        {
            // Get mouse position
            sf::Vector2i mousePos = sf::Mouse::getPosition(window.getWindowHandle());
            // Convert to grid coordinates
            int gridX = mousePos.x / 4; // Assuming each cell is 10x10 pixels
            int gridY = mousePos.y / 4;

            // Apply brush radius
            for (int dx = -BrushRadius; dx <= BrushRadius; ++dx)
            {
                for (int dy = -BrushRadius; dy <= BrushRadius; ++dy)
                {
                    if (dx * dx + dy * dy <= BrushRadius * BrushRadius) // Circle check
                    {
                        grid.clearElement(sf::Vector2i(gridX + dx, gridY + dy));
                    }
                }
            }
        }
	}

    void RenderElementSelection(MonoDomain* domain){

        ImGui::Begin("Elements");
        ImGui::Text("Current Element: %s", currentElement->GetName().c_str());

        if (ImGui::Button("Sand")) {
            currentElement = std::make_unique<SandElement>();
        }
        if (ImGui::Button("Water")) {
            currentElement = std::make_unique<WaterElement>();
        }
        if (ImGui::Button("Concrete")) {
            currentElement = std::make_unique<ConcreteElement>();
        }
		if (ImGui::Button("Scriptable")) {
			currentElement = std::make_unique<ScriptableElement>();
			currentElement->init(dll2, domain);
			scriptableElements.push_back(static_cast<ScriptableElement*>(currentElement.get()));
		}

        ImGui::End();

    }

	void UpdateScriptableElements(std::string dll, MonoDomain* domain) {
		for (auto& element : scriptableElements) {
			element->init(dll, domain);
			dll2 = dll;
		}

	}

	//current element
	std::unique_ptr<PowderElement> currentElement = std::make_unique<SandElement>(); //defaults to sand
	int BrushRadius = 4; //1 is one pixel, 2 is 3 pixels, etc
	int Pressednumber = 0; //Used to track the number of times the key is pressed
	std::vector<ScriptableElement*> scriptableElements; // List of scriptable elements so we can update them at runtime
	std::string dll2 = "Elements/test.dll"; // File to edit
};
