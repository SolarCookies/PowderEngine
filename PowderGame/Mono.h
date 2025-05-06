#pragma once
#include <string>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>

#include "Element/ElementList.h"
#include "PowderGrid.h"

namespace mono {
	MonoDomain* domain = nullptr;

	static PowderGrid* g_Grid = nullptr;

	// Helper to convert MonoString* to std::string
	std::string MonoStringToStdString(MonoString* monoStr) {
		char* utf8 = mono_string_to_utf8(monoStr);
		std::string result(utf8);
		mono_free(utf8);
		return result;
	}


	extern "C" void INTERNAL_Move(int x, int y, int dx, int dy) { if (g_Grid) g_Grid->move(x, y, dx, dy); }
	static void INTERNAL_Swap(int x1, int y1, int x2, int y2) { if (g_Grid) g_Grid->swap(x1, y1, x2, y2); }
	static void INTERNAL_ClearElement(int x, int y) { if (g_Grid) { g_Grid->clearElement(sf::Vector2i(x, y)); } }
	static void INTERNAL_Log(MonoString* message) { std::cout << MonoStringToStdString(message) << std::endl; }

	static int INTERNAL_IsEmpty(int x, int y) {
		return g_Grid->isEmpty(x, y);
	}

	static int INTERNAL_GetInt(int x, int y, MonoString* name) {
		if (!name) return 0;
		std::string converted = MonoStringToStdString(name);
		PowderElement* element = g_Grid->get(x, y);
		return element ? element->GetInt(converted) : 0;
	}

	static float INTERNAL_GetFloat(int x, int y, MonoString* name) {
		if (!name) return 0.0f;
		std::string converted = MonoStringToStdString(name);
		PowderElement* element = g_Grid->get(x, y);
		return element ? element->GetFloat(converted) : 0.0f;
	}

	static bool INTERNAL_GetBool(int x, int y, MonoString* name) {
		if (!name) return false;
		std::string converted = MonoStringToStdString(name);
		PowderElement* element = g_Grid->get(x, y);
		return element ? element->GetBool(converted) : false;
	}

	static int INTERNAL_GetColor(int x, int y, MonoString* name) {
		if (!name) return 0;
		std::string converted = MonoStringToStdString(name);
		PowderElement* element = g_Grid->get(x, y);
		if (!element) return 0;
		sf::Color c = element->GetColor(converted);
		return (c.a << 24) | (c.r << 16) | (c.g << 8) | (c.b);
	}

	static void INTERNAL_SetInt(int x, int y, MonoString* name, int value) {
		if (!name) return;
		std::string converted = MonoStringToStdString(name);
		PowderElement* element = g_Grid->get(x, y);
		if (element) element->SetInt(converted, value);
	}

	static void INTERNAL_SetFloat(int x, int y, MonoString* name, float value) {
		if (!name) return;
		std::string converted = MonoStringToStdString(name);
		PowderElement* element = g_Grid->get(x, y);
		if (element) element->SetFloat(converted, value);
	}

	static void INTERNAL_SetBool(int x, int y, MonoString* name, bool value) {
		if (!name) return;
		std::string converted = MonoStringToStdString(name);
		PowderElement* element = g_Grid->get(x, y);
		if (element) element->SetBool(converted, value);
	}

	static void INTERNAL_SetColor(int x, int y, MonoString* name, int value) {
		if (!name) return;
		std::string converted = MonoStringToStdString(name);
		PowderElement* element = g_Grid->get(x, y);
		if (!element) return;

		sf::Color c = sf::Color(
			(value >> 16) & 0xFF,
			(value >> 8) & 0xFF,
			(value >> 0) & 0xFF,
			(value >> 24) & 0xFF
		);

		element->SetColor(converted, c);
	}

	extern "C" void INTERNAL_Spawn(int x, int y, MonoString* name) {
		if (g_Grid) {
			std::string converted = MonoStringToStdString(name);
			if (elements::GetElementByName(converted, domain, g_Grid)) {
				if (elements::GetElementByName(converted, domain, g_Grid)) {
					g_Grid->set(x, y, elements::GetElementByName(converted, domain, g_Grid));

					//std::cout << "Spawned: " << converted << std::endl;
				}
				else {

					std::cout << "Failed to Spawn: " << converted << std::endl;
				}
			}
		}
	}

	static void registerFunctions(MonoDomain* domain, PowderGrid& grid) {
		g_Grid = &grid;
		// Register the functions
		mono_add_internal_call("PowderGrid::Move", (void*)&INTERNAL_Move);
		mono_add_internal_call("PowderGrid::Swap", (void*)&INTERNAL_Swap);
		mono_add_internal_call("PowderGrid::Kill", (void*)&INTERNAL_ClearElement);
		mono_add_internal_call("PowderGrid::Log", (void*)&INTERNAL_Log);
		mono_add_internal_call("PowderGrid::Spawn", (void*)&INTERNAL_Spawn);
		mono_add_internal_call("PowderGrid::IsEmpty", (void*)&INTERNAL_IsEmpty);

		mono_add_internal_call("PowderGrid::GetInt", (void*)&INTERNAL_GetInt);
		mono_add_internal_call("PowderGrid::GetFloat", (void*)&INTERNAL_GetFloat);
		mono_add_internal_call("PowderGrid::GetBool", (void*)&INTERNAL_GetBool);
		mono_add_internal_call("PowderGrid::GetColor", (void*)&INTERNAL_GetColor);

		mono_add_internal_call("PowderGrid::SetInt", (void*)&INTERNAL_SetInt);
		mono_add_internal_call("PowderGrid::SetFloat", (void*)&INTERNAL_SetFloat);
		mono_add_internal_call("PowderGrid::SetBool", (void*)&INTERNAL_SetBool);
		mono_add_internal_call("PowderGrid::SetColor", (void*)&INTERNAL_SetColor);
	}

	static void init(PowderGrid* grid) {
		// Initialize Mono
		mono_set_dirs(".", ".");
		mono_config_parse(nullptr);
		domain = mono_jit_init("PowderDomain");
		registerFunctions(domain, *grid);
	}
	
	static void cleanup() {
		if (domain) {
			mono_jit_cleanup(domain);
			domain = nullptr;
		}
	}

}
