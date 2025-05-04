#pragma once
#include <string>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>

#include "Elements/PowderElement.h"
#include "PowderGrid.h"

namespace mono {

	static PowderGrid* g_Grid = nullptr;

	extern "C" void INTERNAL_Move(int x, int y, int dx, int dy) {
		if (g_Grid) g_Grid->move(x, y, dx, dy);
	}

	extern "C" void INTERNAL_Swap(int x1, int y1, int x2, int y2) {
		if (g_Grid) g_Grid->swap(x1, y1, x2, y2);
	}
	extern "C" void INTERNAL_ClearElement(int x, int y) {
		if (g_Grid) g_Grid->clearElement(sf::Vector2i(x, y));
	}
	extern "C" void INTERNAL_GetID(int x, int y) {
		if (g_Grid) g_Grid->getID(x, y);
	}
	extern "C" void INTERNAL_GetDensity(int x, int y) {
		if (g_Grid) g_Grid->getDensity(x, y);
	}
	extern "C" void INTERNAL_GetTemperature(int x, int y) {
		if (g_Grid) g_Grid->getTemperature(x, y);
	}
	extern "C" void INTERNAL_GetEnergy(int x, int y) {
		if (g_Grid) g_Grid->getEnergy(x, y);
	}
	extern "C" void INTERNAL_GetName(int x, int y) {
		if (g_Grid) g_Grid->getName(x, y);
	}
	extern "C" void INTERNAL_IsSolid(int x, int y) {
		if (g_Grid) g_Grid->isSolid(x, y);
	}
	extern "C" void INTERNAL_IsEmpty(int x, int y) {
		if (g_Grid) g_Grid->isEmpty(x, y);
	}


	MonoDomain* domain = nullptr;

	static void registerFunctions(MonoDomain* domain, PowderGrid& grid) {
		g_Grid = &grid;
		// Register the functions
		mono_add_internal_call("PowderGrid::Move", (void*)&INTERNAL_Move);
		mono_add_internal_call("PowderGrid::Swap", (void*)&INTERNAL_Swap);
		mono_add_internal_call("PowderGrid::ClearElement", (void*)&INTERNAL_ClearElement);
		mono_add_internal_call("PowderGrid::GetID", (void*)&INTERNAL_GetID);
		mono_add_internal_call("PowderGrid::GetDensity", (void*)&INTERNAL_GetDensity);
		mono_add_internal_call("PowderGrid::GetTemperature", (void*)&INTERNAL_GetTemperature);
		mono_add_internal_call("PowderGrid::GetEnergy", (void*)&INTERNAL_GetEnergy);
		mono_add_internal_call("PowderGrid::GetName", (void*)&INTERNAL_GetName);
		mono_add_internal_call("PowderGrid::IsSolid", (void*)&INTERNAL_IsSolid);
		mono_add_internal_call("PowderGrid::IsEmpty", (void*)&INTERNAL_IsEmpty);

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
