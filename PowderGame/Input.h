#pragma once
#include "World/PowderGrid.h"
#include "View/Window.h"

#include <filesystem>

class Input {
public:
    Input() {}
	~Input() {}

    elements* scripts;

	void TickInput(Window& window, PowderGrid& grid) {
        // Process events
        while (const auto event = window.getWindowHandle().pollEvent()) {
            ImGui::SFML::ProcessEvent(window.getWindowHandle(), *event);

            if (event->is<sf::Event::Closed>()) {
                window.getWindowHandle().close();
            }
        }
        if (IsAdding) {
            return;
        }

        // Handle mouse button events
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            if (currentElement.ID == 0) {
                return;
            }

            
            // Convert pixel to grid coordinates (4x4 pixel cells), offset to center the brush
            int gridX = (mousePos.x / grid.CELL_SIZE) - BrushRadius;
            int gridY = (mousePos.y / grid.CELL_SIZE) - BrushRadius;


            // Random number generator
            std::random_device rd;
            std::mt19937 gen(rd());

            // Apply brush radius
            for (int dx = 0; dx <= BrushRadius * 2; ++dx) {
                for (int dy = 0; dy <= BrushRadius * 2; ++dy) {
                    int relX = dx - BrushRadius;
                    int relY = dy - BrushRadius;

                    if (relX * relX + relY * relY <= BrushRadius * BrushRadius) { // Circle mask
                        int cellX = gridX + dx;
                        int cellY = gridY + dy;

                        // Clamp to bounds
                        if (cellX < 0 || cellY < 0 || cellX >= grid.GRID_WIDTH || cellY >= grid.GRID_HEIGHT)
                            continue;

                        if (grid.get(cellX, cellY)->ID == 0) {
                            short rand = std::uniform_int_distribution<>(0, 32000)(gen);
                            grid.set(cellX, cellY, currentElement);

                            if (grid.get(cellX, cellY)->ID != 0) {
                                grid.get(cellX, cellY)->Seed = rand;

                                if (PowderScript* script = scripts->GetScript(grid.get(cellX, cellY)->ID)) {
                                    script->BeginPlay(cellX, cellY, grid.frame, grid.get(cellX, cellY));
                                }
                            }
                        }
                    }
                }
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        {
            // Get mouse position
            // Convert to grid coordinates
            int gridX = mousePos.x / grid.CELL_SIZE; // Assuming each cell is 10x10 pixels
            int gridY = mousePos.y / grid.CELL_SIZE;

            // Apply brush radius
            for (int dx = -BrushRadius; dx <= BrushRadius; ++dx)
            {
                for (int dy = -BrushRadius; dy <= BrushRadius; ++dy)
                {
                    if (dx * dx + dy * dy <= BrushRadius * BrushRadius) // Circle check
                    {
                        int cellX = gridX + dx;
                        int cellY = gridY + dy;
                        // Clamp to bounds
                        if (cellX < 0 || cellY < 0 || cellX >= grid.GRID_WIDTH || cellY >= grid.GRID_HEIGHT)
                            continue;

                        grid.clearElement(sf::Vector2i(gridX + dx, gridY + dy));
                    }
                }
            }
        }
        if (inputdelay <= 0) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                grid.paused = !grid.paused;
                inputdelay = 1000;
            }
        }
        else {
            inputdelay--;
        }
        
	}

    void RenderElementSelection(PowderGrid& grid){
		ImGui::Begin("Elements", nullptr, ImGuiWindowFlags_MenuBar);


        if (ImGui::BeginMenuBar()) {
			if (currentElement.ID == 0) {
                ImGui::Text("No Element Selected");
            }
            else {
               // ImGui::Text("Selected Element: %s", currentElement->GetString("Name").c_str());
            }
            ImGui::EndMenuBar();
        }

        for (auto & [id, script1] : scripts->ElementScripts) {
			if (ImGui::Button(script1->Name.c_str())) {
                Element temp;
                script1->OnConstruct(&temp);
				currentElement = temp;
                
				std::cout << "Current Element: " << script1->ID << "\n";
			}
		}

        ImGui::End();

    }


	//current element
    Element currentElement; //defaults to null
	int BrushRadius = 4; //1 is one pixel, 2 is 3 pixels, etc
	int Pressednumber = 0; //Used to track the number of times the key is pressed
	bool IsAdding = false; // Used to track if the user is adding an element
    int inputdelay = 0;
    ImVec2 mousePos;
};
