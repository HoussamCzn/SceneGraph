#include "collada.hpp"

#include <fmt/core.h>
#include <span> // std::span

/**
 * @brief Decomposes the transformation matrix of a node into a translation, rotation and scaling.
 * @param node The node to decompose.
 * @return The transformation data of the node.
 */
auto node_transformations(aiNode const* node) noexcept -> transform_data
{
    transform_data data;
    node->mTransformation.Decompose(data.scaling, data.rotation, data.translation);

    return data;
}

auto analyze_nodes(aiScene const* scene, std::string_view node_name) -> std::vector<node_data>
{
    std::vector<node_data> infos;
    std::span const nodes{scene->mRootNode->mChildren, scene->mRootNode->mNumChildren};
    std::ranges::for_each(nodes, [&infos, node_name, scene](auto const* node) {
        if (node_name.empty() || node_name == node->mName.C_Str()) [[likely]]
        {
            auto& info = infos.emplace_back(node->mName.C_Str(), node_transformations(node));
            std::span const node_meshes{node->mMeshes, node->mNumMeshes};
            std::span const scene_meshes{scene->mMeshes, scene->mNumMeshes};
            info.meshes.reserve(node_meshes.size());
            std::ranges::for_each(node_meshes, [&info, scene_meshes](auto const mesh_index) {
                auto const* mesh = scene_meshes[mesh_index];
                info.meshes.emplace_back(mesh->mName.C_Str(), mesh->mNumFaces, mesh->mNumVertices);
            });
        }
    });

    return infos;
}