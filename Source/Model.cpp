#include <iostream>
#include <unordered_map>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <tinyobjloader/tiny_obj_loader.h>

#include "Mesh.hpp"
#include "Model.hpp"
#include "Texture.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

Model LoadModelOBJ(const std::filesystem::path& path, ShaderProgram shaderProgram, const std::string& name,
                   bool flipTexture)
{

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path.string()))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader [Error]: " << reader.Error();
        }
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader [Warning]: " << reader.Warning();
    }

    const tinyobj::attrib_t&                attributes = reader.GetAttrib();
    const std::vector<tinyobj::shape_t>&    shapes     = reader.GetShapes();
    const std::vector<tinyobj::material_t>& materials  = reader.GetMaterials();

    size_t numVertices = attributes.vertices.size() / 3;

    Model model;

    model.name          = name.c_str();
    model.shaderProgram = shaderProgram;

    // Adding Meshes
    for (const auto& shape : shapes)
    {
        std::vector<Vertex>                  vertices = std::vector<Vertex>();
        std::vector<unsigned int>            indices  = std::vector<unsigned>();
        std::unordered_map<Vertex, uint32_t> uniqueVertices;

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

        model.meshes.push_back({.vertices = vertices, .indices = indices, .materialId = shape.mesh.material_ids[0]});
        InitializeMesh(model.meshes.back());
    }

    // Add materials
    for (const auto& material : materials)
    {
        Material modelMaterial;

        if (!material.diffuse_texname.empty())
        {
            modelMaterial.textures.push_back(
                LoadTexture(path.parent_path() / material.diffuse_texname, TextureType::Color, "diffuse", flipTexture));
        }
        if (!material.specular_texname.empty())
        {
            modelMaterial.textures.push_back(LoadTexture(path.parent_path() / material.specular_texname,
                                                         TextureType::NonColor, "specular", flipTexture));
        }

        model.materials.push_back(modelMaterial);
    }

    printf("Loaded Model %s (%d meshes, %d materials)\n", path.string().c_str(), model.meshes.size(),
           model.materials.size());

    return model;
}

void DrawModel(const Model& model) { DrawModel(model, model.shaderProgram); }

void DrawModel(const Model& model, const ShaderProgram shaderProgram)
{
    Transform transform = model.transform;

    glm::mat4 rotation = glm::toMat4(glm::quat(transform.rotation));

    glm::mat4 modelMatrix =
        glm::translate(glm::mat4(1.0f), transform.position) * rotation * glm::scale(glm::mat4(1.0f), transform.scale);

    UseShaderProgram(shaderProgram);
    ShaderSetMat4(shaderProgram, "model", modelMatrix);

    for (auto& mesh : model.meshes)
    {
        DrawMesh(mesh, shaderProgram, model.materials[mesh.materialId]);
    }
}
