#pragma once
#include <SFML/Graphics.hpp>  
#include <SFML/Window.hpp>  
#include <SFML/System.hpp>  

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_internal.h"

#include "powderGrid.h"


class Viewport {
public:
	Viewport() {};

	void ClearRT() {
		viewport.clear(sf::Color::Black);
	}

	void SetGrid(std::shared_ptr<PowderGrid> grid) {
		gridptr = grid;
		UpdateSize();
	}

	void UpdateSize() {
		//resize to imgui window size
		uint32_t width = static_cast<uint32_t>(viewportSize.x);
		uint32_t height = static_cast<uint32_t>(viewportSize.y);

		sf::Vector2u size = { width, height };

		if (viewportSize.x != TempViewportSize.x || viewportSize.y != TempViewportSize.y) {
			if (gridptr != nullptr) {
				gridptr->resize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
			}
			TempViewportSize.x = viewportSize.x;
			TempViewportSize.y = viewportSize.y;
		}
		viewport.resize(size);
	}

	void Draw() {
		if (gridptr == nullptr) {
			return;
		}
		gridptr->draw(viewport);

		ImGui::Begin("Viewport");
		viewportSize = ImGui::GetContentRegionAvail();
		ImGui::Image(viewport);
		
		ImGui::End();
	}


private:
	sf::RenderTexture viewport;
	std::shared_ptr<PowderGrid> gridptr;
	ImVec2 viewportSize = { 800, 600 };
	ImVec2 TempViewportSize = { 800, 600 };
};