#include "Model.hpp"
#include "Shader.hpp"
#include "Texture.hpp"


struct Billboard
{
    Transform transform;
    Shader    shader;

    unsigned int vbo;
    unsigned int vao;

    Texture texture;
};

Billboard LoadBillboard(const std::string& texturePath);
void      DrawBillboard(Billboard& billboard);