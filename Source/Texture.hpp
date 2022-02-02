
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

TextureAsset LoadTexture(const std::filesystem::path& path, TextureType type = TextureType::Color,
                         bool flipTexture = false); // TODO: take textureType as param
TextureAsset LoadTexture(const std::filesystem::path& path, TextureType type, bool flipTexture, const std::string& debugName);
DepthTexture CreateDepthTexture(TextureDimension width, TextureDimension height);
DepthTexture CreateDepthTextureArray(uint16_t shadowMapCount, TextureDimension width, TextureDimension height,
                                     const std::string& debugName = "Depth Texture Array");

DepthTexture CreateDepthCubemap(TextureDimension resolution, const std::string& debugName = "Depth Cubemap");
DepthTexture CreateDepthCubemapArray(uint16_t shadowMapCount, TextureDimension resolution,
                                     const std::string& debugName = "Depth Cubemap Array");

TextureId CreateTexture();
void      DeleteTexture(TextureId textureId);

void BindTexture(unsigned int id, unsigned int slot);
void UnBindTexture(const unsigned int slot);

void BindTextureArray(unsigned int id, unsigned int slot);
void UnBindTextureArray(const unsigned int slot);

void BindCubemapArray(unsigned int id, unsigned int slot);
void UnBindCubemapArray(const unsigned int slot);