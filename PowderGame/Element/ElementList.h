#pragma once

#include "PowderElement.h"
#include "Sand.h"
#include "Gas.h"
#include "Life.h"

class elements {
	public:
	std::unordered_map<short, std::unique_ptr<PowderScript>> ElementScripts;

	void InitScripts() {
		ElementScripts.emplace(1, std::make_unique<Sand>()); //Sand ID = 1
		ElementScripts.emplace(2, std::make_unique<Gas>()); //Gas ID = 2
		ElementScripts.emplace(3, std::make_unique<Life>()); //Life ID = 3
	}

	PowderScript* GetScript(short ID) {
		if (ID == 0) return nullptr;
		if (ElementScripts[ID].get()) {
			return ElementScripts[ID].get();
		}
		return nullptr;
	}


};