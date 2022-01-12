#include "Texture.hpp"

#include <iostream>

#include <glad/glad.h>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

Texture LoadTexture(const std::filesystem::path& path, const TextureType type, const char* name, bool flipTexture)
{

    stbi_set_flip_vertically_on_load(flipTexture);

    std::filesystem::path filePathStr = path;

    TextureId      textureId;
    int            width, height;
    int            componentCount;
    unsigned char* data = stbi_load(filePathStr.make_preferred().string().c_str(), &width, &height, &componentCount, 0);
    bool           isLoaded = false;
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;

        if (componentCount == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (componentCount == 3)
        {
            internalFormat = GL_RGB;
            dataFormat     = GL_RGB;
        }
        else if (componentCount == 4)
        {
            internalFormat = GL_RGBA;
            dataFormat     = GL_RGBA;
        }

        textureId = CreateTexture();
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, internalFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, internalFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "Texture at path: " << path << "loaded (nr: " << componentCount << ", width: " << width
                  << ", height: " << height << "\n";

        isLoaded = true;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << "\n";
    }

    stbi_image_free(data);

    std::string pathStr  = path.string();
    const char* pathCStr = pathStr.c_str();

    return {.path           = pathCStr,
            .name           = name,
            .id             = textureId,
            .type           = type,
            .width          = static_cast<TextureDimension>(width),
            .height         = static_cast<TextureDimension>(height),
            .componentCount = static_cast<uint8_t>(componentCount),
            .isLoaded       = isLoaded};
}

Texture LoadTexture(const std::filesystem::path& path, const char* name, bool flipTexture)
{
    return LoadTexture(path, TextureType::Color, name, flipTexture);
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