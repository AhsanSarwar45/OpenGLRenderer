
#pragma once

#include <filesystem>
#include <string>

#include "Aliases.hpp"

enum class TextureType
{
    Color,
    NonColor
};

struct TextureAsset
{
    TextureId        id;
    TextureType      type;
    TextureDimension width;
    TextureDimension height;
    uint8_t          componentCount;
    bool             isLoaded;
    bool             isFlipped;
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
    TextureDimension width; // TODO: Don't need these
    TextureDimension height;
};

TextureAsset LoadTexture(const std::filesystem::path& path, bool flipTexture = false);
TextureAsset LoadTexture(const std::filesystem::path& path, const TextureType type, bool flipTexture = false);
DepthTexture CreateDepthTexture(TextureDimension width, TextureDimension height);

DepthTexture CreateCubeMapTexture(TextureDimension resolution);

TextureId CreateTexture();
void      DeleteTexture(TextureId textureId);

void BindTexture(unsigned int id, unsigned int slot);
void UnBindTexture(const unsigned int slot);