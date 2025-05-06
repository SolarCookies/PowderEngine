#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <iostream>

struct ScriptMetadata {
    MonoAssembly* assembly = nullptr;
    MonoImage* image = nullptr;
    MonoClass* klass = nullptr;
    std::unordered_map<std::string, MonoMethod*> methods;
};

class ScriptCache {
public:
    static ScriptMetadata* LoadScript(const std::string& dllPath, std::string& className, MonoDomain* domain) {
        auto it = cache.find(dllPath);
        if (it != cache.end()) {
            if (!it->second.klass || !it->second.image) {
                std::cerr << "[ScriptCache] Incomplete metadata for: " << dllPath << ", skipping.\n";
                return nullptr;
            }
            return &it->second;
        }

        // Add empty entry to prevent concurrent reentry
        cache[dllPath] = {};
        ScriptMetadata& meta = cache[dllPath];

        meta.assembly = mono_domain_assembly_open(domain, dllPath.c_str());
        if (!meta.assembly) {
            std::cerr << "[ScriptCache] Failed to load assembly: " << dllPath << std::endl;
            cache.erase(dllPath);
            return nullptr;
        }

        meta.image = mono_assembly_get_image(meta.assembly);
        if (!meta.image) {
            std::cerr << "[ScriptCache] Failed to get image from: " << dllPath << std::endl;
            cache.erase(dllPath);
            return nullptr;
        }

        meta.klass = mono_class_from_name(meta.image, "", className.c_str());
        if (!meta.klass) {
            std::cerr << "[ScriptCache] Failed to get class: " << className << " from: " << dllPath << std::endl;
            cache.erase(dllPath);
            return nullptr;
        }

        // Pre-cache common methods
        const std::vector<std::string> methodNames = { "Update", "GetColor", "GetName", "GetTemperature", "GetDensity" };
        for (const auto& methodName : methodNames) {
            MonoMethod* method = mono_class_get_method_from_name(meta.klass, methodName.c_str(), 0);
            if (method) {
                meta.methods[methodName] = method;
            }
            else {
                std::cerr << "[ScriptCache] Warning: Method " << methodName << " not found in " << dllPath << std::endl;
            }
        }

        if (!logged.insert(dllPath).second) {
            std::cout << "[ScriptCache] Loaded script: " << dllPath << std::endl;
        }

        return &cache[dllPath];
    }

    static MonoMethod* GetMethod(const std::string& dllPath, const std::string& methodName) {
        auto it = cache.find(dllPath);
        if (it != cache.end()) {
            auto mit = it->second.methods.find(methodName);
            if (mit != it->second.methods.end()) {
                return mit->second;
            }
        }
        return nullptr;
    }

private:
    static inline std::unordered_map<std::string, ScriptMetadata> cache;
    static inline std::unordered_set<std::string> logged;
};