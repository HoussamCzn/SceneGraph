#pragma once

#include <assimp/scene.h> // aiScene, aiNode, aiMesh
#include <cstdint>        // std::uint32_t
#include <string>         // std::string
#include <string_view>    // std::string_view
#include <vector>         // std::vector

/**
 * @brief Contains the transformation data of a node.
 * Each node has a transformation matrix, which can be decomposed.
 */
struct transform_data
{
    aiVector3D translation; ///< The translation of the node.
    aiQuaternion rotation;  ///< The rotation of the node.
    aiVector3D scaling;     ///< The scaling of the node.
};

/**
 * @brief Contains the information of a mesh.
 */
struct mesh_data
{
    std::string name;               ///< The name of the mesh.
    std::uint32_t face_count{0U};   ///< The number of faces of the mesh.
    std::uint32_t vertex_count{0U}; ///< The number of vertices of the mesh.
};

/**
 * @brief Contains the information of a node.
 */
struct node_data
{
    std::string name;              ///< The name of the node.
    transform_data transform;      ///< The transformation data of the node.
    std::vector<mesh_data> meshes; ///< The meshes of the node.
};

/**
 * @brief Analyzes the meshes of a scene. Empty names will analyze all meshes.
 * @param scene The scene to analyze.
 * @param node_name The name of the node to analyze.
 * @param mesh_name The name of the mesh to analyze.
 * @return The mesh information.
 */
auto analyze_nodes(aiScene const* scene, std::string_view node_name = "") -> std::vector<node_data>;