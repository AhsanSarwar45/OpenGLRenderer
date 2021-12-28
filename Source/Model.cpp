#include <iostream>
#include <unordered_map>

#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <tinyobjloader/tiny_obj_loader.h>

#include "Mesh.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "glm/gtc/quaternion.hpp"

Model LoadModelOBJ(const std::filesystem::path& path, const std::string& name)
{

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path.string()))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    const tinyobj::attrib_t&                attributes = reader.GetAttrib();
    const std::vector<tinyobj::shape_t>&    shapes     = reader.GetShapes();
    const std::vector<tinyobj::material_t>& materials  = reader.GetMaterials();

    size_t                    numVertices = attributes.vertices.size() / 3;
    std::vector<Vertex>       vertices    = std::vector<Vertex>();
    std::vector<unsigned int> indices     = std::vector<unsigned>();

    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    for (const auto& shape : shapes)
    {
        // For each shape, loop through its indices.
        for (const auto& index : shape.mesh.indices)
        {
            // Construct a new (x, y, z) position for the current mesh index.
            glm::vec3 position{attributes.vertices[3 * index.vertex_index + 0],
                               attributes.vertices[3 * index.vertex_index + 1],
                               attributes.vertices[3 * index.vertex_index + 2]};

            // Construct a new (u, v) texture coordinate for the current mesh index.
            glm::vec2 texCoord{attributes.texcoords[2 * index.texcoord_index + 0],
                               1.0f - attributes.texcoords[2 * index.texcoord_index + 1]};

            glm::vec3 normal{attributes.normals[3 * index.normal_index + 0],
                             attributes.normals[3 * index.normal_index + 1],
                             attributes.normals[3 * index.normal_index + 2]};

            // Construct a vertex with the extracted data.
            Vertex vertex{position, normal, texCoord};

            // This will help deduplicate vertices - we maintain a hash map where a
            // vertex is used as a unique key with its value being which index can
            // be used to locate the vertex. The vertex is only added if it has not
            // been added before.
            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }

    // for (size_t i = 0; i < numVertices; i++)
    // {
    //     size_t vertexIndex   = i * 3;
    //     size_t texCoordIndex = i * 2;

    //     vertices[i] = {.Position  = glm::vec3(attrib.vertices[vertexIndex], attrib.vertices[vertexIndex + 1],
    //                                          attrib.vertices[vertexIndex + 2]),
    //                    .Normal    = glm::vec3(attrib.normals[vertexIndex], attrib.normals[vertexIndex + 1],
    //                                        attrib.normals[vertexIndex + 2]),
    //                    .TexCoords = glm::vec2(attrib.texcoords[texCoordIndex], attrib.texcoords[texCoordIndex + 1])};
    // }

    std::cout << "Model: " << path << "\n";
    std::cout << "Vertices: " << vertices.size() << "\n";
    std::cout << "Indices: " << indices.size() << "\n";

    Model model;

    model.meshes.push_back({.Vertices = vertices, .Indices = indices});
    model.name = name;

    for (auto& mesh : model.meshes)
    {
        InitializeMesh(mesh);
    }

    model.textures.push_back(
        LoadTexture((path.parent_path() / materials[0].diffuse_texname).string(), TextureType::Color));

    return model;
}

void DrawModel(Model& model, Shader& shader)
{
    Transform transform   = model.transform;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix           = glm::translate(modelMatrix, transform.position);
    modelMatrix           = glm::scale(modelMatrix, transform.scale);

    UseShader(shader);
    ShaderSetMat4(shader, "model", modelMatrix);

    for (auto& mesh : model.meshes)
    {
        DrawMesh(mesh, shader, model.textures[0]);
    }
}
