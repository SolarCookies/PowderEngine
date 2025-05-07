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
    
	std::unique_ptr<PowderElement> clone();

	void OnConstruct(MonoDomain* inDomain, MonoClass* klass) override; //Runs when element is created
	void BeginPlay(int x, int y, int& frame) override; //When placed in world
	void Tick(int x, int y, int& frame) override; //Runs every frame
	void SafeTick(int x, int y, int& frame) override; //Runs every frame but cant access move Fuctions like move, kill or swap
	void Sleep(int x, int y, int& frame) override; //Runs when sleep

	int GetInt(std::string Name) override;
	float GetFloat(std::string Name) override;
	bool GetBool(std::string Name) override;
	sf::Color GetColor(std::string Name) override;

	void SetInt(const std::string& Name, int value) override;
	void SetFloat(const std::string& Name, float value) override;
	void SetBool(const std::string& Name, bool value) override;
	void SetColor(const std::string& Name, const sf::Color& value) override;

	bool IsDirty() override;

	void CallMethodIfExists(MonoObject* instance, const char* methodName, int x, int y, int frame) {
		if (!instance) return;

		MonoClass* klass = mono_object_get_class(instance);
		if (!klass) {
			std::cerr << "[Mono] Failed to get class from instance for method " << methodName << "\n";
			return;
		}
		MonoMethod* method = mono_class_get_method_from_name(klass, methodName, 3);
		if (!method) return;

		void* args[3] = { &x, &y, &frame };
		MonoObject* exception = nullptr;
		mono_runtime_invoke(method, instance, args, &exception);

		if (exception) {
			MonoString* str = mono_object_to_string(exception, nullptr);
			char* msg = mono_string_to_utf8(str);
			std::cerr << "[Mono Exception] " << methodName << ": " << msg << "\n";
			mono_free(msg);
		}
	}

	MonoClassField* GetField(const std::string& name) {
		if (!klass) return nullptr;

		auto it = Fields.find(name);
		if (it != Fields.end()) {
			return it->second;
		}

		MonoClassField* field = mono_class_get_field_from_name(klass, name.c_str());
		if (field) {
			Fields[name] = field; // cache it
		}

		return field;
	}

	template<typename T>
	T GetFieldValue(std::string name) {
		T value{};
		if (MonoClassField* field = GetField(name)) {
			mono_field_get_value(scriptInstance, field, &value);
		}
		return value;
	}

};

