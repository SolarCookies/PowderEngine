#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>
#include "PowderElement.h"
#include <iostream>


class ScriptableElement : public PowderElement {
public:
    void update(int x, int y, PowderGrid& grid) override;
    std::unique_ptr<PowderElement> clone() const override { return std::make_unique<ScriptableElement>(*this); }
    sf::Color getColor() override;
    std::string GetName() const override { return "Wall"; }
    sf::Color color = sf::Color(0, 0, 0, 255);
    bool isSolid() const override { return true; } // Concrete is solid (Cant be swapped by water)
    bool isEmpty() const override { return false; }
	void init(const std::string& scriptPath, MonoDomain* domain) override {
		std::cout << "Loading script: " << scriptPath << std::endl;
		this->domain = domain;
		this->scriptInstance = nullptr;
		// Load the script
		MonoAssembly* assembly = mono_domain_assembly_open(domain, scriptPath.c_str());
		if (!assembly) {
			throw std::runtime_error("Failed to load script: " + scriptPath);
		}
		MonoImage* image = mono_assembly_get_image(assembly);
		MonoClass* klass = mono_class_from_name(image, "", "ScriptableElement");
		if (!klass) {
			throw std::runtime_error("Failed to find class in script: ScriptableElement");
		}
		// Create an instance of the class
		MonoObject* obj = mono_object_new(domain, klass);
		MonoMethod* updateMethod = mono_class_get_method_from_name(klass, "Update", 2);
		mono_runtime_object_init(obj);
		scriptInstance = obj;
	}

};

