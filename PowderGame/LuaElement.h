#pragma once

#include "PowderElement.h"
#include <LuaCpp.hpp> // Assuming LuaCpp is the library you're using for Lua integration
#include <string>

class LuaElement : public PowderElement {
public:
    LuaElement(const std::string& luaFilePath);

    void update(int x, int y, PowderGrid& grid, std::mt19937 gen) override;
    sf::Color getColor(std::mt19937 gen) override;

private:
    Lua::State luaState; // Lua state for this element
    std::string Path;    // Path to the Lua script
};