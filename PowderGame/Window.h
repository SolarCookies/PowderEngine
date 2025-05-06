#pragma once
#include <SFML/Graphics.hpp>  
#include <SFML/Window.hpp>  
#include <SFML/System.hpp>  

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_internal.h"

#include "Viewport.h"
#include "powderGrid.h"

class Window {
public:
	Window() {}

	~Window() {
		ImGui::SFML::Shutdown();
		window.close();
	}

	void InitWindow(sf::Vector2u WindowSize, std::string TitleText, std::shared_ptr<PowderGrid> Grid, int framerate = 60) {
		window.create(sf::VideoMode(WindowSize), TitleText, sf::Style::Default);
		window.setFramerateLimit(framerate);
		ImGui::SFML::Init(window);

		viewport.SetGrid(Grid);
		viewport.UpdateSize();

		gridptr = Grid;

		// Enable docking
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Viewports

	}
	sf::RenderWindow& getWindowHandle() {
		return window;
	}

	void InitImGuiFrame(sf::Clock Delta) {
		ImGui::SFML::Update(window, Delta.restart());
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		window.clear(sf::Color::Black);

		gridptr->update();

		viewport.UpdateSize();
	}

	void RenderImGui() {
		viewport.Draw();

		
	}

	void Render() {
		ImGui::SFML::Render(window);
		window.display();
	}

	Viewport viewport;
private:
	sf::RenderWindow window;
	std::shared_ptr<PowderGrid> gridptr;

};

