
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

struct Texture
{
    TextureId        id;
    unsigned int     internalFormat;
    unsigned int     format;
    unsigned int     type;
    TextureDimension width;
    TextureDimension height;
    std::string      debugName;
};

struct FramebufferTextureData
{
    const char*  name;
    unsigned int internalFormat;
};

TextureAsset LoadTexture(const std::filesystem::path& path, TextureType type = TextureType::Color,
                         bool flipTexture = false); // TODO: take textureType as param
TextureAsset LoadTexture(const std::filesystem::path& path, TextureType type, bool flipTexture, const std::string& debugName);

Texture CreateDepthTexture(TextureDimension width, TextureDimension height, const std::string& debugName = "Depth Texture");
Texture CreateDepthTextureArray(uint16_t shadowMapCount, TextureDimension width, TextureDimension height,
                                const std::string& debugName = "Depth Texture Array");

Texture CreateDepthCubemap(TextureDimension resolution, const std::string& debugName = "Depth Cubemap");
Texture CreateDepthCubemapArray(uint16_t shadowMapCount, TextureDimension resolution, const std::string& debugName = "Depth Cubemap Array");

TextureId CreateTexture();
void      DeleteTexture(TextureId textureId);

void BindTexture(unsigned int id, unsigned int slot);
void UnBindTexture(const unsigned int slot);

void BindTextureArray(unsigned int id, unsigned int slot);
void UnBindTextureArray(const unsigned int slot);

void BindCubemapArray(unsigned int id, unsigned int slot);
void UnBindCubemapArray(const unsigned int slot);