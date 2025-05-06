#pragma once
#include "../PowderGrid.h"  
#include "ScriptableElement.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>

#include <filesystem>


namespace elements {

	MonoAssembly* CompileScript(std::string& csPath, std::string& dllPath, MonoDomain* domain, std::string& reference) {
		std::string cscPath = "\"C:\\Windows\\Microsoft.NET\\Framework64\\v4.0.30319\\csc.exe\"";
		std::string command = cscPath + " /target:library /out:" + dllPath;
		if (!reference.empty()) {
			command += " /reference:" + reference;
		}
		command += " " + csPath;

		int result = std::system(command.c_str());
		if (result != 0) {
			std::cerr << "Failed to compile " << csPath << " with csc.exe (exit code: " << result << ")\n";
			return nullptr;
		}

		MonoAssembly* assembly = mono_domain_assembly_open(domain, dllPath.c_str());
		if (!assembly) {
			std::cerr << "Failed to load " << dllPath << "\n";
			return nullptr;
		}

		return assembly;
	}

	struct ElementClass {
		std::string name;
		std::string path;
		std::string dllPath;
		std::shared_ptr<PowderElement> Class;

		// New cached Mono data
		MonoAssembly* assembly = nullptr;
		MonoImage* image = nullptr;
		MonoClass* klass = nullptr;
	};

	std::vector<ElementClass> ElementList;

	void UpdateElementList(std::string PathToElements) {
		//Get all .cs files in the directory and add them to the list
		for (const auto& entry : std::filesystem::directory_iterator(PathToElements)) {
			if (entry.path().extension() == ".cs") {
				ElementClass element;
				element.name = entry.path().stem().string();
				element.path = entry.path().string();
				bool contains = false;
				//If list contains item already ignore
				for (ElementClass& element2 : ElementList) {
					if (element2.name == element.name) {
						contains = true;
					}
				}
				if (contains == false) {
					ElementList.push_back(element);
				}
			}
		}

	}

	void RecompileElement(std::string& name, PowderGrid* grid, MonoDomain* domain) {
		for (ElementClass& element : ElementList) {
			if (element.name == name) {
				std::random_device rd;  // Obtain a random number from hardware
				std::mt19937 gen(rd()); // Seed the generator
				int rand = std::uniform_int_distribution<>(0, 1000000)(gen);
				std::string basePath = element.path.substr(0, element.path.find_last_of('\\'));
				std::string fileName = element.path.substr(element.path.find_last_of('\\') + 1);
				fileName = fileName.substr(0, fileName.find_last_of('.')) + std::to_string(rand) + ".dll";

				std::string dllPath = basePath + "\\dlls\\" + fileName;
				std::string reference = "PowderGrid.dll";

				MonoAssembly* assembly = CompileScript(element.path, dllPath, domain, reference);
				if (!assembly) return;

				element.dllPath = dllPath;
				element.assembly = assembly;
				element.image = mono_assembly_get_image(assembly);
				element.klass = mono_class_from_name(element.image, "", "ScriptableElement");

				std::cout << "Recompiled " << element.name << "\n";
			}
		}
	}

	std::unique_ptr<PowderElement> GetElementByName(std::string & name, MonoDomain * domain, PowderGrid * grid) {
		for (auto& element : ElementList) {
			if (element.name == name) {
				if (element.Class) {
					return element.Class->clone();
				}
				if (element.dllPath.empty()) {
					std::string temp = element.name;
					RecompileElement(temp, grid, domain);
				}

				std::cout << "Loading script dll: " << element.dllPath << std::endl;

				std::unique_ptr<PowderElement> currentElement = std::make_unique<ScriptableElement>();
				currentElement->OnConstruct(domain, element.klass);

				element.Class = std::move(currentElement);
				return element.Class->clone();
			}
		}
		return nullptr;
	}


};