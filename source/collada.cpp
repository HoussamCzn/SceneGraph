#include "collada.hpp"

#include <span> // std::span

auto transformations(aiNode const* node) noexcept -> transform_data
{
    transform_data data;
    node->mTransformation.Decompose(data.scaling, data.rotation, data.translation);
    return data;
}

auto analyze_meshes(aiScene const* scene, std::string_view mesh_name) noexcept -> std::vector<mesh_info>
{
    std::span const meshes{scene->mMeshes, scene->mNumMeshes};
    std::vector<mesh_info> infos;

    if (!mesh_name.empty())
    {
        auto const* mesh = scene->mRootNode->FindNode(mesh_name.data());

        if (mesh != nullptr) [[likely]]
        {
            mesh_info info{};
            info.name = mesh->mName.C_Str();
            info.transform = transformations(mesh);
            infos.push_back(info);
        }
    }
    else
    {
        infos.reserve(meshes.size());

        for (auto const* mesh : meshes)
        {
            mesh_info info{};
            info.name = mesh->mName.C_Str();
            auto const* node = scene->mRootNode->FindNode(mesh->mName);
            if (node != nullptr) { info.transform = transformations(node); }
            infos.push_back(info);
        }
    }

    return infos;
}
