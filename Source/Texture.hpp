
#pragma once

#include <filesystem>
#include <string>

enum class TextureType
{
    Color,
    NonColor
};

using TextureDimensions = uint16_t;
using TextureId         = unsigned int;

struct Texture
{
    const char*       path;
    const char*       name;
    TextureId         id;
    TextureType       type;
    TextureDimensions width;
    TextureDimensions height;
    uint8_t           componentCount;
    bool              isLoaded;
};

struct DepthTexture
{
    TextureId         id;
    TextureDimensions width;
    TextureDimensions height;
};

Texture      LoadTexture(const std::filesystem::path& path, const char* name = "diffuse", bool flipTexture = false);
Texture      LoadTexture(const std::filesystem::path& path, const TextureType type, const char* name = "diffuse",
                         bool flipTexture = false);
DepthTexture CreateDepthTexture(TextureDimensions width, TextureDimensions height);

void BindTexture(unsigned int id, unsigned int slot);
void UnBindTexture(const unsigned int slot);