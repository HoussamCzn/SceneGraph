#include "cli.hpp"

#include "collada.hpp" // analyze_nodes, node_data

#include <CLI/CLI.hpp>          // CLI::App, CLI11_PARSE
#include <assimp/Importer.hpp>  // Assimp::Importer
#include <assimp/postprocess.h> // aiProcess_Triangulate
#include <fmt/core.h>           // fmt::println
#include <span>                 // std::span

auto print_infos(std::span<node_data const> nodes) -> void
{
    std::ranges::for_each(nodes, [](auto const& node) {
        fmt::println("==================================================");
        fmt::println("Node: {}", node.name);
        fmt::println("--------------------------------------------------");
        fmt::println("Translation: ({:.2f}, {:.2f}, {:.2f})", node.transform.translation.x, node.transform.translation.y,
                     node.transform.translation.z);
        fmt::println("Rotation:    ({:.2f}, {:.2f}, {:.2f}, {:.2f})", node.transform.rotation.x, node.transform.rotation.y,
                     node.transform.rotation.z, node.transform.rotation.w);
        fmt::println("Scaling:     ({:.2f}, {:.2f}, {:.2f})", node.transform.scaling.x, node.transform.scaling.y,
                     node.transform.scaling.z);

        if (!node.meshes.empty()) [[unlikely]]
        {
            fmt::println("\nMeshes:");
            std::ranges::for_each(node.meshes, [](auto const& mesh) {
                fmt::println("\t  Name:     {}", mesh.name);
                fmt::println("\t  Faces:    {}", mesh.face_count);
                fmt::println("\t  Vertices: {}", mesh.vertex_count);
            });
        }
        else { fmt::println("No meshes are attached to this node."); }

        fmt::println("==================================================\n");
    });
}

auto run_cli(int argc, char** argv) noexcept -> int
{
    CLI::App app{"Analyze COLLADA files."};
    std::string path;
    std::string node_name;

    app.add_option("FILE", path, "Path to the COLLADA file")->required();
    app.add_option("-n, --node", node_name, "Specify a mesh to analyze (default: all meshes)");
    CLI11_PARSE(app, argc, argv);

    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(path, aiProcess_Triangulate);

    if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0U || scene->mRootNode == nullptr) [[unlikely]]
    {
        fmt::println(stderr, "Failed to load file \"{}\"", path);

        return EXIT_FAILURE;
    }

    auto infos = analyze_nodes(scene, node_name);

    if (infos.empty()) [[unlikely]]
    {
        if (node_name.empty()) { fmt::println(stderr, "No nodes found."); }
        else { fmt::println(stderr, "No nodes with name \"{}\" found.", node_name); }
    }

    print_infos(infos);

    return EXIT_SUCCESS;
}
