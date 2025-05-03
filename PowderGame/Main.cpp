#include <LuaCpp.hpp>

#include "Window.h"
#include "Input.h"
#include "Style.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_internal.h"

#include <iostream>
#include <random>


int main() {

    std::shared_ptr<PowderGrid> grid = std::make_shared<PowderGrid>();

	Window window;
	window.InitWindow(sf::Vector2u(800, 600), "PowderSim", grid, 9999);

    Input input;

	int fps = 0;
	int fpscount = 0;
	sf::Clock fpsclock;
    sf::Clock delta;
    while (window.getWindowHandle().isOpen())
    {
		window.InitImGuiFrame(delta);

        input.TickInput(window, *grid);

		Style::SetStyle();

        window.RenderImGui();

        input.RenderElementSelection();

		if (fpsclock.getElapsedTime().asSeconds() > 1) {
			fps = fpscount;
            fpscount = 0;
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
        ImGui::Text("Element Count: %d", grid->getCount());
        ImGui::End();

        window.Render();
		delta.restart();
    }

	window.~Window();
    return 0;
}