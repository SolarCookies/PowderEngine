
#include "Window.h"
#include "Input.h"
#include "Style.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_internal.h"
#include "TextEditor.h"
#include "Element/ElementList.h"

#include <iostream>
#include <random>
#include <windows.h>
#include <fstream>
#include <cstdlib>
#include <filesystem>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>

#include "mono.h"

bool CompileScript(const std::string& csPath, const std::string& dllPath, MonoDomain* domain, const std::string& reference = "") {
    std::string cscPath = "\"C:\\Windows\\Microsoft.NET\\Framework64\\v4.0.30319\\csc.exe\"";

    std::string command = cscPath + " /target:library /out:" + dllPath;
    if (!reference.empty()) {
        command += " /reference:" + reference;
    }
    command += " " + csPath;

    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to compile " << csPath << " with csc.exe (exit code: " << result << ")\n";
        return false;
    }

    MonoAssembly* assembly = mono_domain_assembly_open(domain, dllPath.c_str());
    if (!assembly) {
        std::cerr << "Failed to load " << dllPath << "\n";
        return false;
    }

    return true;
}



int main() {

    std::shared_ptr<PowderGrid> grid = std::make_shared<PowderGrid>();

    mono::init(grid.get());

	//Compile and run PowderGrid.cs
	if (!CompileScript("PowderGrid.cs", "PowderGrid.dll", mono::domain)) {
		return 1;
    }
    else {
		std::cout << "Compiled and loaded " << "PowderGrid.dll" << " successfully.\n";
    }


	Window window;
	window.InitWindow(sf::Vector2u(800, 600), "PowderSim", grid, 9999);
    window.getWindowHandle().setKeyRepeatEnabled(false);


    Input input;

    //test text editor
    static TextEditor editor;

    editor.SetHandleKeyboardInputs(true);
	editor.SetLanguageDefinition(TextEditor::LanguageDefinition::C());
	editor.SetPalette(TextEditor::GetDarkPalette());
	editor.SetShowWhitespaces(true);
	editor.SetReadOnly(false);

    static const char* fileToEdit = "Elements\\test.cs";

    {
        std::ifstream t(fileToEdit);
        if (t.good())
        {
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            editor.SetText(str);
        }
    }

    int fps = 0;
    int fpscount = 0;
    sf::Clock fpsclock;
    sf::Clock delta;
    while (window.getWindowHandle().isOpen())
    {
		window.InitImGuiFrame(delta);

        input.mousePos = window.viewport.mousePos;
        input.TickInput(window, *grid, editor);

		Style::SetStyle();

        window.RenderImGui();

		input.RenderElementSelection(mono::domain, *grid);

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
        ImGui::Text("Mouse Pos: %d", window.viewport.mousePos);

        ImGui::InputInt("Brush Radius", &input.BrushRadius);


        ImGui::End();

		bool open = true;
		ImGui::Begin("Text Editor", &open, ImGuiWindowFlags_MenuBar);
        //add menu bar with a compile button
		if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                for (elements::ElementClass& element : elements::ElementList) {
                    if (ImGui::Button(element.name.c_str())) {
                        fileToEdit = element.path.c_str();
                        std::ifstream t(fileToEdit);
                        if (t.good())
                        {
                            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                            editor.SetText(str);
                        }
                    }
                }
                ImGui::EndMenu();
            }
			if (ImGui::Button("Save")) {
				std::ofstream outFile(fileToEdit);
				if (outFile.is_open()) {
					outFile << editor.GetText();
					outFile.close();
					std::cout << "Saved " << fileToEdit << "\n";
				}
				else {
					std::cerr << "Failed to save " << fileToEdit << "\n";
				}
			}
			if (ImGui::MenuItem("Compile")) {
				std::string csRandomID = std::to_string(rand() % 1000000);
				std::string csPath = "Elements\\test.cs";
				std::string dllPath = "Elements\\test" + csRandomID + ".dll";
				CompileScript(csPath, dllPath, mono::domain, "PowderGrid.dll");

			}
			ImGui::EndMenuBar();
		}

        ImVec2 size = ImGui::GetContentRegionAvail();
		editor.Render("Text Editor1", size, false);
		ImGui::End();

        window.Render();

		delta.restart();
    }

	window.~Window();
    return 0;
}