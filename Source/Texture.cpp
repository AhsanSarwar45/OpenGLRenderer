#include "Texture.hpp"

#include <iostream>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

Texture::Texture(const std::string& path) { Load(path); }

Texture::Texture(const std::string& path, TextureType type) : m_Path(path), m_Type(type) { Load(path); }

Texture::Texture() {}

Texture::~Texture() {}

void Texture::Load(const std::string& path)
{
    stbi_set_flip_vertically_on_load(false);

    int            width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;

        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = GL_RGB;
            dataFormat     = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = GL_RGBA;
            dataFormat     = GL_RGBA;
        }

        glGenTextures(1, &m_Id);
        glBindTexture(GL_TEXTURE_2D, m_Id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }

    stbi_image_free(data);
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_Id);
}