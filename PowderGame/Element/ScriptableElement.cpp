#include "ScriptableElement.h"

std::unique_ptr<PowderElement> ScriptableElement::clone() {
    auto clone = std::make_unique<ScriptableElement>();

    clone->OnConstruct(domain, klass);
    return clone;
}

void ScriptableElement::OnConstruct(MonoDomain* inDomain, MonoClass* inClass) {
    klass = inClass;
    domain = inDomain;

    scriptInstance = mono_object_new(domain, klass);
    if (!scriptInstance) {
        return;
    }

    mono_runtime_object_init(scriptInstance); // call constructor for c# object
}

void ScriptableElement::BeginPlay(int x, int y, int& frame) {
    CallMethodIfExists(scriptInstance, "BeginPlay", x, y, frame);
}

void ScriptableElement::Tick(int x, int y, int& frame) {
    // Optional user logic public "void SleepTick(int x, int y, int frame)"
    CallMethodIfExists(scriptInstance, "Tick", x, y, frame);
}

void ScriptableElement::Sleep(int x, int y, int& frame)
{
    // Optional user logic "public void SleepTick(int x, int y, int frame)"
    CallMethodIfExists(scriptInstance, "SleepTick", x, y, frame);
}

int ScriptableElement::GetInt(std::string name) {
    if (!klass || !scriptInstance) return 0;
    MonoClassField* field = mono_class_get_field_from_name(klass, name.c_str());
    if (!field) return 0;
    int value = 0;
    mono_field_get_value(scriptInstance, field, &value);
    return value;
}

float ScriptableElement::GetFloat(std::string name) {
    if (!klass || !scriptInstance) return 0.0f;
    MonoClassField* field = mono_class_get_field_from_name(klass, name.c_str());
    if (!field) return 0.0f;
    float value = 0.0f;
    mono_field_get_value(scriptInstance, field, &value);
    return value;
}

bool ScriptableElement::GetBool(std::string name) {
    if (!klass || !scriptInstance) return false;
    MonoClassField* field = mono_class_get_field_from_name(klass, name.c_str());
    if (!field) return false;
    bool value = false;
    mono_field_get_value(scriptInstance, field, &value);
    return value;
}


sf::Color ScriptableElement::GetColor(std::string name) {
    if (!klass || !scriptInstance) return sf::Color::Magenta;
    MonoClassField* field = mono_class_get_field_from_name(klass, name.c_str());
    if (!field) return sf::Color::Magenta;
    int rawColor = 0;
    mono_field_get_value(scriptInstance, field, &rawColor);
    return sf::Color(
        (rawColor >> 16) & 0xFF,
        (rawColor >> 8) & 0xFF,
        (rawColor >> 0) & 0xFF,
        (rawColor >> 24) & 0xFF
    );
}

void ScriptableElement::SetInt(const std::string& name, int value) {
    if (!klass || !scriptInstance) return;
    MonoClassField* field = mono_class_get_field_from_name(klass, name.c_str());
    if (field) mono_field_set_value(scriptInstance, field, &value);
}

void ScriptableElement::SetFloat(const std::string& name, float value) {
    if (!klass || !scriptInstance) return;
    MonoClassField* field = mono_class_get_field_from_name(klass, name.c_str());
    if (field) mono_field_set_value(scriptInstance, field, &value);
}

void ScriptableElement::SetBool(const std::string& name, bool value) {
    if (!klass || !scriptInstance) return;
    MonoClassField* field = mono_class_get_field_from_name(klass, name.c_str());
    if (field) mono_field_set_value(scriptInstance, field, &value);
}

void ScriptableElement::SetColor(const std::string& name, const sf::Color& color) {
    if (!klass || !scriptInstance) return;
    int packed = (color.a << 24) | (color.r << 16) | (color.g << 8) | (color.b);
    MonoClassField* field = mono_class_get_field_from_name(klass, name.c_str());
    if (field) mono_field_set_value(scriptInstance, field, &packed);
}
