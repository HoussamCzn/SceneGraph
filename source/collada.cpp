#include "collada.hpp"

#include <assimp/Exporter.hpp> // Assimp::Exporter
#include <assimp/matrix4x4.h>  // aiMatrix4x4
#include <span>                // std::span
#include <stack>               // std::stack

/**
 * @brief Decomposes the transformation matrix of a node into a translation, rotation and scaling.
 * @param node The node to decompose.
 * @return The transformation data of the node.
 */
auto node_transformations(aiNode const* const node) noexcept -> transform_data
{
    transform_data data;
    node->mTransformation.Decompose(data.scaling, data.rotation, data.translation);

    return data;
}

auto apply_transformations(aiScene const* scene, aiNode* root_node, aiMatrix4x4 const& initial_transform) noexcept -> void
{
    std::stack<std::pair<aiNode* const, aiMatrix4x4 const>> stack;
    stack.emplace(root_node, initial_transform);

    while (!stack.empty())
    {
        auto [node, parent_transform] = stack.top();
        stack.pop();

        auto const global_transform = parent_transform * node->mTransformation;
        std::span const scene_meshes{scene->mMeshes, scene->mNumMeshes};
        std::span const node_meshes{node->mMeshes, node->mNumMeshes};

        std::ranges::for_each(node_meshes, [scene_meshes, global_transform](auto const mesh_index) {
            auto const* const mesh = scene_meshes[mesh_index];
            std::span const vertices{mesh->mVertices, mesh->mNumVertices};
            std::ranges::for_each(vertices, [global_transform](auto& vertex) { vertex *= global_transform; });

            if (mesh->HasNormals())
            {
                std::span const normals{mesh->mNormals, mesh->mNumVertices};
                std::ranges::for_each(normals, [global_transform](auto& normal) { normal *= global_transform; });
            }
        });

        std::span const children{node->mChildren, node->mNumChildren};
        std::ranges::for_each(children, [&stack, global_transform](auto* child) { stack.emplace(child, global_transform); });

        node->mTransformation = aiMatrix4x4{};
    }
}

auto write_scene(aiScene const* const scene, std::filesystem::path const& path) noexcept -> std::optional<std::string>
{
    Assimp::Exporter exporter;
    if (exporter.Export(scene, "collada", path.c_str()) == AI_SUCCESS) [[likely]] { return std::nullopt; }

    return exporter.GetErrorString();
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