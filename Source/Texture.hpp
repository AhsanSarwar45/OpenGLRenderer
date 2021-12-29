
#pragma once

#include <filesystem>
#include <string>

enum class TextureType
{
    Color,
    NonColor
};

using TextureDimensions = uint16_t;

struct Texture
{
    const char*       path;
    unsigned int      id;
    TextureType       type;
    TextureDimensions width;
    TextureDimensions height;
    uint8_t           componentCount;
    bool              isLoaded;
};

Texture LoadTexture(const std::filesystem::path& path);
Texture LoadTexture(const std::filesystem::path& path, const TextureType type);

void BindTexture(unsigned int id, unsigned int slot);
