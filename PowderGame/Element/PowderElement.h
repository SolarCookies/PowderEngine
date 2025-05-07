#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <random>
#include <limits.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>

const int CELL_SIZE = 4;  // Each element is a 4x4 square
inline int GRID_WIDTH = 128;
inline int GRID_HEIGHT = 128;


class PowderElement {
public:

	virtual std::unique_ptr<PowderElement> clone() {return std::make_unique<PowderElement>(*this);
	}

	virtual void OnConstruct(MonoDomain* inDomain, MonoClass* klass) {} //Runs when element is created
	virtual void BeginPlay(int x, int y, int& frame) {} //When placed in world
	virtual void Tick(int x, int y, int& frame) {} //Runs every frame
	virtual void SafeTick(int x, int y, int& frame) {} //Runs every frame but cant access Fuctions like move or swap
	virtual void Sleep(int x, int y, int& frame) {} //Runs when sleep

	virtual int GetInt(std::string Name) { return -1; }
	virtual float GetFloat(std::string Name) { return -1.0; }
	virtual bool GetBool(std::string Name) { return false; }
	virtual sf::Color GetColor(std::string Name) { return sf::Color(255, 0, 255, 255); }

	virtual void SetInt(const std::string& Name, int value) {}
	virtual void SetFloat(const std::string& Name, float value) {}
	virtual void SetBool(const std::string& Name, bool value) {}
	virtual void SetColor(const std::string& Name, const sf::Color& value) {}

	virtual bool isEmpty() const { return false; }
	virtual bool IsDirty() { return false; }

	MonoDomain* domain;
	MonoClass* klass;
	MonoObject* scriptInstance;

protected:
	std::string MonoStringToStdString(MonoString* monoStr) {
		char* utf8 = mono_string_to_utf8(monoStr);
		std::string result(utf8);
		mono_free(utf8);
		return result;
	}

	MonoClassField* dirtyField = nullptr;
	std::unordered_map<std::string, MonoClassField*> Fields;

};