
#pragma once

#include <string>

enum class TextureType
{
    Color,
    NonColor
};

class Texture
{

  public:
    Texture(const std::string& path);
    Texture(const std::string& path, TextureType type);
    Texture();
    ~Texture();

    void Load(const std::string& path);
    void Bind(unsigned int slot) const;

    TextureType GetType() const { return m_Type; }
    int         GetID() const { return m_Id; }
    void        SetType(TextureType type) { m_Type = type; }

  private:
    unsigned int m_Id;
    TextureType  m_Type;
    std::string  m_Path;
};