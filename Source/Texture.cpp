#include "Texture.hpp"

#include <iostream>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

Texture LoadTexture(const std::filesystem::path& path, const TextureType type, const char* name, bool flipTexture)
{
    stbi_set_flip_vertically_on_load(flipTexture);

    unsigned int   id;
    int            width, height;
    int            componentCount;
    unsigned char* data     = stbi_load(path.string().c_str(), &width, &height, &componentCount, 0);
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
            internalFormat = (type == TextureType::NonColor) ? GL_SRGB : GL_RGB;
            dataFormat     = GL_RGB;
        }
        else if (componentCount == 4)
        {
            internalFormat = (type == TextureType::NonColor) ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat     = GL_RGBA;
        }

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, internalFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, internalFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // std::cout << "Texture at path: " << path << "loaded (nr: " << componentCount << ", width: " << width
        //           << ", height: " << height << "\n";

        isLoaded = true;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << "\n";
    }

    stbi_image_free(data);

    return {.path           = path.string().c_str(),
            .name           = name,
            .id             = id,
            .type           = type,
            .width          = width,
            .height         = height,
            .componentCount = componentCount,
            .isLoaded       = isLoaded};
}

Texture LoadTexture(const std::filesystem::path& path, const char* name, bool flipTexture)
{
    return LoadTexture(path, TextureType::Color, name, flipTexture);
}

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