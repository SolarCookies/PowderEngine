#pragma once
#include "Element/ElementList.h"
#include "PowderGrid.h"
#include "Window.h"
#include "TextEditor.h"

#include <filesystem>

class Input {
public:
    Input() { elements::UpdateElementList("Elements\\"); }
	~Input() {}

	void TickInput(Window& window, PowderGrid& grid, TextEditor& TE) {
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
            if (currentElement == nullptr) {
                return;
            }

            
            // Convert pixel to grid coordinates (4x4 pixel cells), offset to center the brush
            int gridX = (mousePos.x / 4) - BrushRadius;
            int gridY = (mousePos.y / 4) - BrushRadius;

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
                        if (cellX < 0 || cellY < 0 || cellX >= GRID_WIDTH || cellY >= GRID_HEIGHT)
                            continue;

                        if (grid.get(cellX, cellY) == nullptr || grid.get(cellX, cellY)->isEmpty()) {
                            int rand = std::uniform_int_distribution<>(0, 1000000)(gen);
                            grid.set(cellX, cellY, currentElement->clone());

                            PowderElement* placed = grid.get(cellX, cellY);
                            if (placed != nullptr) {
                                placed->SetInt("ID", rand);
                                placed->BeginPlay(cellX, cellY, grid.frame);
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
            int gridX = mousePos.x / 4; // Assuming each cell is 10x10 pixels
            int gridY = mousePos.y / 4;

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
                        if (cellX < 0 || cellY < 0 || cellX >= GRID_WIDTH || cellY >= GRID_HEIGHT)
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

    void RenderElementSelection(MonoDomain* domain, PowderGrid& grid){
		ImGui::Begin("Elements", nullptr, ImGuiWindowFlags_MenuBar);


        if (ImGui::BeginMenuBar()) {
			if (ImGui::Button("Add")) {
                //Add popup to enter name
				IsAdding = true;


			}
            if (currentElement == nullptr) {
                ImGui::Text("No Element Selected");
            }
            else {
               // ImGui::Text("Selected Element: %s", currentElement->GetString("Name").c_str());
            }
            ImGui::EndMenuBar();
        }

        if (IsAdding) {
			ImGui::Begin("Add Element", &IsAdding);
			ImGui::Text("Enter the name of the element:");
			static char name[128] = "";
			ImGui::InputText("Name", name, sizeof(name));
            if (ImGui::Button("Create")) {
                // Build source and destination paths
                std::string src = "Template.cs";
                std::string dst = "Elements\\" + std::string(name) + ".cs";

                try {
                    std::filesystem::copy_file(src, dst, std::filesystem::copy_options::overwrite_existing);
                }
                catch (std::filesystem::filesystem_error& e) {
                    std::cerr << "File copy failed: " << e.what() << std::endl;
                }

                IsAdding = false;
                elements::UpdateElementList("Elements\\");
            }
			ImGui::End();
        }

		

        for (elements::ElementClass& element : elements::ElementList) {
			if (ImGui::Button(element.name.c_str())) {
				currentElement = elements::GetElementByName(element.name, domain, &grid);
				std::cout << "Current Element: " << element.name << "\n";
			}
		}

        ImGui::End();

    }


	//current element
    std::unique_ptr<PowderElement> currentElement; //defaults to null
	int BrushRadius = 4; //1 is one pixel, 2 is 3 pixels, etc
	int Pressednumber = 0; //Used to track the number of times the key is pressed
	std::string dll2 = "Elements/test.dll"; // File to edit
	bool IsAdding = false; // Used to track if the user is adding an element
    int inputdelay = 0;
    ImVec2 mousePos;
};
