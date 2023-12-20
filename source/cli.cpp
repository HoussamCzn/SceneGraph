#include "cli.hpp"

#include "collada.hpp" // analyze_nodes, node_data

#include <CLI/CLI.hpp>          // CLI::App, CLI11_PARSE
#include <assimp/Importer.hpp>  // Assimp::Importer
#include <assimp/postprocess.h> // aiProcess_Triangulate
#include <fmt/core.h>           // fmt::print
#include <span>                 // std::span

auto print_infos(std::span<node_data const> nodes) -> void
{
    std::ranges::for_each(nodes, [](auto const& node) {
        fmt::print("==================================================\n");
        fmt::print("Node: {}\n", node.name);
        fmt::print("--------------------------------------------------\n");
        fmt::print("Translation: ({:.2f}, {:.2f}, {:.2f})\n", node.transform.translation.x, node.transform.translation.y,
                     node.transform.translation.z);
        fmt::print("Rotation:    ({:.2f}, {:.2f}, {:.2f}, {:.2f})\n", node.transform.rotation.x, node.transform.rotation.y,
                     node.transform.rotation.z, node.transform.rotation.w);
        fmt::print("Scaling:     ({:.2f}, {:.2f}, {:.2f})\n", node.transform.scaling.x, node.transform.scaling.y,
                     node.transform.scaling.z);

        if (!node.meshes.empty()) [[unlikely]]
        {
            fmt::print("\nMeshes:\n");
            std::ranges::for_each(node.meshes, [](auto const& mesh) {
                fmt::print("\t  Name:     {}\n", mesh.name);
                fmt::print("\t  Faces:    {}\n", mesh.face_count);
                fmt::print("\t  Vertices: {}\n", mesh.vertex_count);
            });
        }
        else { fmt::print("No meshes are attached to this node.\n"); }

        fmt::print("==================================================\n\n");
    });
}

auto run_cli(int argc, char** argv) noexcept -> int
{
    CLI::App app{"Analyze COLLADA files."};
    std::string input_path;
    std::string output_path;
    std::string node_name;

    app.add_option("FILE", input_path, "Path to the COLLADA file")->required();
    app.add_option("-n, --node", node_name, "Specify a node to analyze (default: all nodes)");
    app.add_option("-o, --output", output_path, "Path to the output file (default: overwrite input file)");

    CLI11_PARSE(app, argc, argv);

    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(input_path, aiProcess_Triangulate);

    if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0U || scene->mRootNode == nullptr) [[unlikely]]
    {
        fmt::print(stderr, "{}\n", importer.GetErrorString());

        return EXIT_FAILURE;
    }

    if (!output_path.empty())
    {
        apply_transformations(scene, scene->mRootNode, scene->mRootNode->mTransformation);

        if (auto const write_result = write_scene(scene, output_path)) [[unlikely]]
        {
            fmt::print(stderr, "{}\n", *write_result);

            return EXIT_FAILURE;
        }

        fmt::print("Successfully wrote file \"{}\"\n", output_path);

        return EXIT_SUCCESS;
    }

    auto const infos = analyze_nodes(scene, node_name);

    if (infos.empty()) [[unlikely]]
    {
        if (node_name.empty()) { fmt::print(stderr, "No nodes found.\n"); }
        else { fmt::print(stderr, "No nodes with name \"{}\" found.\n", node_name); }
    }

    print_infos(infos);

    return EXIT_SUCCESS;
}
