#include "Texture.hpp"

#include <iostream>

#include <glad/glad.h>
#include <string>

#include "ResourceManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

TextureAsset LoadTexture(const std::filesystem::path& texturePath, const TextureType type, bool flipTexture, const std::string& debugName)
{
    std::filesystem::path fullPath = ResourceManager::GetInstance().GetRootPath() / texturePath;
    stbi_set_flip_vertically_on_load(flipTexture);

    TextureId      textureId;
    int            width, height;
    int            componentCount;
    unsigned char* data     = stbi_load(fullPath.make_preferred().string().c_str(), &width, &height, &componentCount, 0);
    bool           isLoaded = false;
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;

        if (componentCount == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (componentCount == 2)
        {
            internalFormat = GL_RG;
        }
        else if (componentCount == 3)
        {
            if (type == TextureType::Color)
            {
                internalFormat = GL_SRGB;
            }
            else
            {
                internalFormat = GL_RGB;
            }
            dataFormat = GL_RGB;
        }
        else if (componentCount == 4)
        {
            if (type == TextureType::Color)
            {
                internalFormat = GL_SRGB_ALPHA;
            }
            else
            {
                internalFormat = GL_RGBA;
            }
            dataFormat = GL_RGBA;
        }

        textureId = CreateTexture();
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, internalFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, internalFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        isLoaded = true;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << fullPath << "\n";
    }

    stbi_image_free(data);

    glObjectLabel(GL_TEXTURE, textureId, strlen(debugName.c_str()), debugName.c_str());

    return {.id             = textureId,
            .type           = type,
            .width          = static_cast<TextureDimension>(width),
            .height         = static_cast<TextureDimension>(height),
            .componentCount = static_cast<uint8_t>(componentCount),
            .isLoaded       = isLoaded,
            .isFlipped      = flipTexture};
}

TextureAsset LoadTexture(const std::filesystem::path& path, TextureType type, bool flipTexture)
{
    return LoadTexture(path, type, flipTexture, path.filename().string());
}

DepthTexture CreateDepthTexture(TextureDimension width, TextureDimension height)
{
    TextureId depthMapId = CreateTexture();
    glBindTexture(GL_TEXTURE_2D, depthMapId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    return {
        .id     = depthMapId,
        .width  = width,
        .height = height,
    };
}

DepthTexture CreateDepthTextureArray(uint16_t shadowMapCount, TextureDimension width, TextureDimension height, const std::string& debugName)
{
    TextureId depthMapId = CreateTexture();

    /*create the depth texture with a 16-bit depth internal format*/
    glGenTextures(1, &depthMapId);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMapId);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT16, width, height, shadowMapCount, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    /*set up the appropriate filtering and wrapping modes*/
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    glObjectLabel(GL_TEXTURE, depthMapId, strlen(debugName.c_str()), debugName.c_str());

    return {
        .id     = depthMapId,
        .width  = width,
        .height = height,
    };
}

DepthTexture CreateDepthCubemap(TextureDimension resolution, const std::string& debugName)
{

    TextureId depthCubeMapId = CreateTexture();
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapId);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                     NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);

    glObjectLabel(GL_TEXTURE, depthCubeMapId, strlen(debugName.c_str()), debugName.c_str());

    return {
        .id     = depthCubeMapId,
        .width  = resolution,
        .height = resolution,
    };
}

DepthTexture CreateDepthCubemapArray(uint16_t shadowMapCount, TextureDimension resolution, const std::string& debugName)
{

    TextureId depthCubeMapArrayId = CreateTexture();

    /*create the depth texture with a 16-bit depth internal format*/
    glGenTextures(1, &depthCubeMapArrayId);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubeMapArrayId);
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT16, resolution, resolution, shadowMapCount * 6, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, NULL);

    /*set up the appropriate filtering and wrapping modes*/
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glObjectLabel(GL_TEXTURE, depthCubeMapArrayId, strlen(debugName.c_str()), debugName.c_str());

    return {
        .id     = depthCubeMapArrayId,
        .width  = resolution,
        .height = resolution,
    };
}

TextureId CreateTexture()
{
    TextureId texture;
    glGenTextures(1, &texture);

    return texture;
}
void DeleteTexture(TextureId textureId) { glDeleteTextures(1, &textureId); }

void BindTexture(const unsigned int id, const unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void UnBindTexture(const unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void BindTextureArray(unsigned int id, unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

void UnBindTextureArray(const unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void BindCubemapArray(unsigned int id, unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, id);
}

void UnBindCubemapArray(const unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);
}
