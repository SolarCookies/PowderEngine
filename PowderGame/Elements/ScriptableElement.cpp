#include "ScriptableElement.h"

/*
void ScriptableElement::init(const std::string& scriptPath, MonoDomain* domain) {
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
	MonoMethod* updateMethod = mono_class_get_method_from_name(klass, "Update", 3);
	mono_runtime_object_init(obj);
	scriptInstance = obj;
}


*/

#include "../PowderGrid.h"

void ScriptableElement::update(int x, int y, PowderGrid& grid)
{
	if (scriptInstance) {
		MonoMethod* method = mono_class_get_method_from_name(mono_object_get_class(scriptInstance), "Update", 3);
		if (!method) {
			std::cout << "MonoMethod for Update(int, int) not found\n";
			return;
		}
		int gridPtr = reinterpret_cast<intptr_t>(&grid); // or cast however you want
		void* args[3] = { &x, &y, &gridPtr };
		mono_runtime_invoke(method, scriptInstance, args, nullptr);
	}
}

sf::Color ScriptableElement::getColor()
{
	if (scriptInstance) {
		MonoMethod* method = mono_class_get_method_from_name(mono_object_get_class(scriptInstance), "GetColor", 0);
		if (!method) {
			std::cout << "MonoMethod for GetColor() not found\n";
			return sf::Color(255, 255, 255, 255);
		}

		MonoObject* exception = nullptr;
		MonoObject* result = mono_runtime_invoke(method, scriptInstance, nullptr, &exception);

		if (exception) {
			MonoString* str = mono_object_to_string(exception, nullptr);
			char* msg = mono_string_to_utf8(str);
			std::cerr << "C# Exception in GetColor: " << msg << "\n";
			mono_free(msg);
			return sf::Color(255, 255, 255, 255);
		}

		int32_t colorValue = *(int32_t*)mono_object_unbox(result);

		// ARGB unpacking (assuming colorValue = 0xAARRGGBB)
		return sf::Color(
			(colorValue >> 16) & 0xFF, // R
			(colorValue >> 8) & 0xFF,  // G
			colorValue & 0xFF,         // B
			(colorValue >> 24) & 0xFF  // A
		);
	}

	return sf::Color(255, 255, 255, 255);
}