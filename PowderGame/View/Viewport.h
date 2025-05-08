#pragma once
#include <SFML/Graphics.hpp>  
#include <SFML/Window.hpp>  
#include <SFML/System.hpp>  

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_internal.h"

#include "../World/PowderGrid.h"


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

		//borderless imgui window
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		//set background color to black
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar()) {
			if (gridptr->paused) {
				if (ImGui::Button("Play")) {
					gridptr->paused = false;
				}
			}
			else {
				if (ImGui::Button("Pause")) {
					gridptr->paused = true;
				}
			}

			ImGui::EndMenuBar();
		}
		viewportSize = ImGui::GetContentRegionAvail();
		ImVec2 imagePos = ImGui::GetCursorScreenPos();      // Top-left of the image
		ImVec2 mousePos2 = ImGui::GetIO().MousePos;          // Absolute screen mouse position
		mousePos = ImVec2(mousePos2.x - imagePos.x, mousePos2.y - imagePos.y);
		ImGui::Image(viewport);
		//ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		
		ImGui::End();

	}

	ImVec2 mousePos;
private:
	sf::RenderTexture viewport;
	std::shared_ptr<PowderGrid> gridptr;
	ImVec2 viewportSize = { 800, 600 };
	ImVec2 TempViewportSize = { 800, 600 };
};