
#include "View/Window.h"
#include "Input.h"
#include "View/Style.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_internal.h"
#include "Element/ElementList.h"

#include <iostream>
#include <random>
#include <windows.h>
#include <fstream>
#include <cstdlib>
#include <filesystem>

int main() {

    std::shared_ptr<PowderGrid> grid = std::make_shared<PowderGrid>();

    elements e;
    e.InitScripts();

    grid->scripts = &e;

	Window window;
	window.InitWindow(sf::Vector2u(800, 600), "PowderSim", grid, 9999);
    window.getWindowHandle().setKeyRepeatEnabled(false);


    Input input;

    input.scripts = &e;

    int fps = 0;
    int ElementCount = 0;
    int fpscount = 0;
    sf::Clock fpsclock;
    sf::Clock delta;
    while (window.getWindowHandle().isOpen())
    {
		window.InitImGuiFrame(delta);

        input.mousePos = window.viewport.mousePos;
        input.TickInput(window, *grid);

		Style::SetStyle();

        window.RenderImGui();

		input.RenderElementSelection(*grid);

		if (fpsclock.getElapsedTime().asSeconds() > 1) {
			fps = fpscount;
            fpscount = 0;
            ElementCount = grid->getCount();
			fpsclock.restart();
        }
        else {
            fpscount++;
        }

        ImGui::Begin("Details");
        if (fps > 0) {
            ImGui::Text("FPS: %d", fps);
        }
        else {
            ImGui::Text("FPS: calculating...");
        }
        ImGui::Text("Grid Size: %d x %d", grid->getSize().x, grid->getSize().y);
        ImGui::Text("Max Element Count: %d", (grid->getSize().x +1) * (grid->getSize().y+1));
        ImGui::Text("Element Count: %d", ElementCount);

        ImGui::Text("Mouse Pos x: %d, y: %d", window.viewport.mousePos.x, window.viewport.mousePos.y);

        ImGui::InputInt("Brush Radius", &input.BrushRadius);


        ImGui::End();
        window.Render();
    }

	window.~Window();
    return 0;
}