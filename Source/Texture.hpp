
#pragma once

#include <string>

enum class TextureType
{
    Color,
    NonColor
};

using TextureDimensions = uint16_t;

struct Texture
{
    unsigned int      Id;
    TextureType       Type;
    std::string       Path;
    TextureDimensions Width;
    TextureDimensions Height;
    uint8_t           ComponentCount;
    bool              IsLoaded;
};

Texture LoadTexture(const std::string& path);
Texture LoadTexture(const std::string& path, const TextureType type);

void BindTexture(unsigned int id, unsigned int slot);
