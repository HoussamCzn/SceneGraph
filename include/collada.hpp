#pragma once

#include <assimp/scene.h> // aiScene, aiNode, aiMesh
#include <string>         // std::string
#include <string_view>    // std::string_view
#include <vector>         // std::vector

/**
 * @brief Contains the transformation data of a node.
 * Each node has a transformation matrix, which can be decomposed into a translation, rotation and scaling.
 */
struct transform_data
{
    aiVector3D translation; ///< The translation of the node.
    aiQuaternion rotation;  ///< The rotation of the node.
    aiVector3D scaling;     ///< The scaling of the node.
};

/**
 * @brief Contains the name and transformation data of a mesh.
 */
struct mesh_info
{
    std::string name;         ///< The name of the mesh.
    transform_data transform; ///< The transformation data of the mesh.
};

/**
 * @brief Analyzes the meshes of a COLLADA file.
 * @param scene The scene of the COLLADA file.
 * @param mesh_name The name of the mesh to analyze. If empty, all meshes will be analyzed.
 * @return A vector containing the mesh infos.
 */
auto analyze_meshes(aiScene const* scene, std::string_view mesh_name = "") noexcept -> std::vector<mesh_info>;

/**
 * @brief Decomposes the transformation matrix of a node into a translation, rotation and scaling.
 * @param node The node to decompose.
 * @return The transformation data of the node.
 */
auto transformations(aiNode const* node) noexcept -> transform_data;
