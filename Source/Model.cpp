#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <tinyobjloader/tiny_obj_loader.h>

#include "Material.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "ResourceManager.hpp"
#include "Texture.hpp"

using namespace ModelInternal;

std::shared_ptr<Model> LoadModel(const std::filesystem::path& path, std::shared_ptr<Material> material, const std::string& name,
                                 bool flipTexture)
{
    std::filesystem::path fullPath = ResourceManager::GetInstance().GetRootPath() / path;

    std::shared_ptr<Model> model;

    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(fullPath.string().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                                             aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

    if (pScene)
    {
        model = ParseScene(pScene, fullPath, flipTexture);
    }
    else
    {
        printf("Error parsing '%s': '%s'\n", fullPath.string().c_str(), Importer.GetErrorString());
        model = nullptr;
    }

    model->name = name.c_str();

    return model;
}

void SetMaterial(std::shared_ptr<Model> model, std::shared_ptr<Material> material)
{
    model->materials.push_back(material);

    int materialId = model->materials.size() - 1;

    for (auto& mesh : model->meshes)
    {
        mesh->materialId = materialId;
    }
}

namespace ModelInternal
{
std::shared_ptr<Model> ParseScene(const aiScene* scene, const std::filesystem::path& path, bool flipTexture)
{
    std::shared_ptr<Model> model = std::make_shared<Model>();

    model->meshes    = std::vector<std::shared_ptr<Mesh>>(scene->mNumMeshes);
    model->materials = std::vector<std::shared_ptr<Material>>(scene->mNumMaterials);

    // Initialize the meshes in the scene one by one

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        model->meshes[i] = ParseMesh(scene->mMeshes[i]);
    }

    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        std::shared_ptr<Material> material = std::make_shared<Material>();

        const aiMaterial* rawMaterial = scene->mMaterials[i];

        // aiString texturePath;
        // if (AI_SUCCESS == rawMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath))
        // {

        //     material->textures.push_back(LoadTexture(path.parent_path() / texturePath.C_Str(), "diffuse", flipTexture));
        // }
        // if (AI_SUCCESS == rawMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texturePath))
        // {
        //     material->textures.push_back(LoadTexture(path.parent_path() / texturePath.C_Str(), "specular", flipTexture));
        // }
        // if (AI_SUCCESS == rawMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath))
        // {

        //     material->textures.push_back(LoadTexture(path.parent_path() / texturePath.C_Str(), "albedo", flipTexture));
        // }
        // if (AI_SUCCESS == rawMaterial->GetTexture(aiTextureType_METALNESS, 0, &texturePath))
        // {
        //     material->textures.push_back(LoadTexture(path.parent_path() / texturePath.C_Str(), "metalness", flipTexture));
        // }
        // if (AI_SUCCESS == rawMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texturePath))
        // {

        //     material->textures.push_back(LoadTexture(path.parent_path() / texturePath.C_Str(), "roughness", flipTexture));
        // }
        // if (AI_SUCCESS == rawMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath))
        // {
        //     material->textures.push_back(LoadTexture(path.parent_path() / texturePath.C_Str(), "ao", flipTexture));
        // }

        model->materials[i] = material;
    }

    return model;
}

std::shared_ptr<Mesh> ParseMesh(const aiMesh* rawMesh)
{
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    mesh->materialId           = rawMesh->mMaterialIndex;

    MeshData meshData = {.vertices = std::vector<Vertex>(rawMesh->mNumVertices), .indices = std::vector<unsigned>(rawMesh->mNumFaces * 3)};

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0; i < rawMesh->mNumVertices; i++)
    {
        const aiVector3D* pos       = &(rawMesh->mVertices[i]);
        const aiVector3D* normal    = &(rawMesh->mNormals[i]); // todo check if normals present
        const aiVector3D* texCoord  = rawMesh->HasTextureCoords(0) ? &(rawMesh->mTextureCoords[0][i]) : &Zero3D;
        const aiVector3D* tangent   = &(rawMesh->mTangents[i]);
        const aiVector3D* bitangent = &(rawMesh->mBitangents[i]);

        meshData.vertices[i].Position  = {pos->x, pos->y, pos->z};
        meshData.vertices[i].Normal    = {normal->x, normal->y, normal->z};
        meshData.vertices[i].TexCoord  = {texCoord->x, texCoord->y};
        meshData.vertices[i].Tangent   = {tangent->x, tangent->y, tangent->z};
        meshData.vertices[i].Bitangent = {bitangent->x, bitangent->y, bitangent->z};
    }

    for (unsigned int i = 0; i < rawMesh->mNumFaces; i++)
    {
        const aiFace& face = rawMesh->mFaces[i];

        int startIndex = 3 * i;

        meshData.indices[(startIndex) + 0] = face.mIndices[0];
        meshData.indices[(startIndex) + 1] = face.mIndices[1];
        meshData.indices[(startIndex) + 2] = face.mIndices[2];
    }

    InitializeMesh(mesh, meshData);

    // printf("id %d, num vertices %d, num indices %d\n", mesh->materialId, mesh->vertices.size(),
    // mesh->indices.size());

    return mesh;
}

} // namespace ModelInternal
