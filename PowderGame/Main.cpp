#include <LuaCpp.hpp>

#include "Window.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_internal.h"

#include "PowderGrid.h"  
#include "SandElement.h"
#include "WaterElement.h"
#include "ConcreteElement.h"

#include <iostream>
#include <random>


int main() {

    std::shared_ptr<PowderGrid> grid = std::make_shared<PowderGrid>();
    // Basic SFML setup  
	Window window;
	window.InitWindow(sf::Vector2u(800, 600), "PowderSim", grid, 9999); // Create the window with a size of 800x600 and a title of "Powder Simulation"
    //window.setFramerateLimit(60);

	int BrushRadius = 4; //1 is one pixel, 2 is 3 pixels, etc


    //current element
	std::unique_ptr<PowderElement> currentElement = std::make_unique<SandElement>(); //defaults to sand

    // Initialize FPS counter
    sf::Clock clock;
    static int frameCount = 0;
    static sf::Time elapsedTime = clock.getElapsedTime();
	int fps = 0;

    sf::Clock delta;
    // Run the program as long as the window is open
    while (window.getWindowHandle().isOpen())
    {
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
                        if (grid->get(gridX + dx, gridY + dy) == nullptr || grid->get(gridX + dx, gridY + dy)->isEmpty()) {
                            //random int using <random>
							std::random_device rd;  // Obtain a random number from hardware
							std::mt19937 gen(rd()); // Seed the generator
							int rand = std::uniform_int_distribution<>(0, 1000000)(gen); // Random int between 0 and 1000000
                            grid->set(gridX + dx, gridY + dy, currentElement->clone());
							if (grid->get(gridX + dx, gridY + dy) != nullptr) {
								grid->get(gridX + dx, gridY + dy)->id = rand; // Set the ID of the element
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
                        grid->clearElement(sf::Vector2i(gridX + dx, gridY + dy));
                    }
                }
            }
        }

		window.InitImGuiFrame(delta);

		window.RenderImGui();

		// ImGui window
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


		ImGui::End();

        // Calculate and display FPS in console
        elapsedTime = clock.getElapsedTime();
        frameCount++;

        if (elapsedTime.asSeconds() >= 1.0f) {
			fps = frameCount;
            frameCount = 0;
            clock.restart();
        }
        

        ImGui::Begin("Details");

        ImGui::Text("FPS: %d", fps);
       ImGui::Text("Grid Size: %d x %d", grid->getSize().x, grid->getSize().y);
		ImGui::Text("Element Count: %d", grid->getCount());

        ImGui::End();



		window.Render();


        


    }

	window.~Window();
    return 0;
}