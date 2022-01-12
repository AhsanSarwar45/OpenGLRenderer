
#pragma once

#include <filesystem>
#include <string>

#include "Aliases.hpp"

enum class TextureType
{
    Color,
    NonColor
};

struct Texture
{
    const char*      path;
    const char*      name;
    TextureId        id;
    TextureType      type;
    TextureDimension width;
    TextureDimension height;
    uint8_t          componentCount;
    bool             isLoaded;
};

struct TextureInternalData
{
    TextureId    id;
    unsigned int internalFormat;
    unsigned int format;
    unsigned int type;
};

struct DepthTexture
{
    TextureId        id;
    TextureDimension width;
    TextureDimension height;
};

Texture      LoadTexture(const std::filesystem::path& path, const char* name = "diffuse", bool flipTexture = false);
Texture      LoadTexture(const std::filesystem::path& path, const TextureType type, const char* name = "diffuse",
                         bool flipTexture = false);
DepthTexture CreateDepthTexture(TextureDimension width, TextureDimension height);

TextureId CreateTexture();
void      DeleteTexture(TextureId textureId);

void BindTexture(unsigned int id, unsigned int slot);
void UnBindTexture(const unsigned int slot);