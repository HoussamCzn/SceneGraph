#include "cli.hpp"

#include "collada.hpp" // analyze_meshes, mesh_infos

#include <CLI/CLI.hpp>          // CLI::App, CLI11_PARSE
#include <assimp/Importer.hpp>  // Assimp::Importer
#include <assimp/postprocess.h> // aiProcess_Triangulate
#include <fmt/core.h>           // fmt::println

auto print_mesh_infos(std::vector<mesh_info> const& meshes) -> void
{
    for (auto const& mesh : meshes)
    {
        fmt::println("Mesh: {}", mesh.name);
        fmt::println("\tTranslation: ({}, {}, {})", mesh.transform.translation.x, mesh.transform.translation.y,
                     mesh.transform.translation.z);
        fmt::println("\tRotation: ({}, {}, {}, {})", mesh.transform.rotation.x, mesh.transform.rotation.y,
                     mesh.transform.rotation.z, mesh.transform.rotation.w);
        fmt::println("\tScaling: ({}, {}, {})", mesh.transform.scaling.x, mesh.transform.scaling.y, mesh.transform.scaling.z);
    }
}

auto run_cli(int argc, char** argv) noexcept -> int
{
    CLI::App app{"Analyze COLLADA files."};
    std::string path;
    std::string specific_mesh;

    app.add_option("FILE", path, "Path to the COLLADA file")->required();
    app.add_option("-m,--mesh", specific_mesh, "Specify a mesh to analyze (default: all meshes)");

    CLI11_PARSE(app, argc, argv);

    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(path, aiProcess_Triangulate);

    if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0U || scene->mRootNode == nullptr) [[unlikely]]
    {
        fmt::println(stderr, "Failed to load file: {}", path);

        return EXIT_FAILURE;
    }

    auto infos = analyze_meshes(scene, specific_mesh);
    if (infos.empty()) [[unlikely]] { fmt::println(stderr, "No meshes with name \"{}\" found.", specific_mesh); }
    print_mesh_infos(infos);

    return EXIT_SUCCESS;
}
