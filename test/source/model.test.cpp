#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>
#include <fstream>
#include <glm/glm.hpp>
#include <string_view>

using std::operator""sv;

static constexpr auto mock_collada =
    R"(<?xml version="1.0" encoding="UTF-8"?>
<COLLADA
	xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
	<library_geometries>
		<geometry id="parent-cube">
			<mesh>
				<source id="parent-cube-coordonnees">
					<float_array id="parent-cube-coordonnees-donnees" count="24">
                        -10 -10 -10  10 -10 -10 -10  10 -10  10  10 -10
                        -10 -10  10  10 -10  10 -10  10  10  10  10  10
                    </float_array>
					<technique_common>
						<accessor count="8" offset="0" source="#parent-cube-coordonnees-donnees" stride="3">
							<param name="X" type="float"/>
							<param name="Y" type="float"/>
							<param name="Z" type="float"/>
						</accessor>
					</technique_common>
				</source>
				<vertices id="parent-cube-sommets">
					<input semantic="POSITION" source="#parent-cube-coordonnees"/>
				</vertices>
				<triangles count="12">
					<input offset="0" semantic="VERTEX" source="#parent-cube-sommets"/>
					<p>
                        3 1 0    2 3 0    3 7 1    5 1 7    6 5 7    5 6 4
                        4 6 2    4 2 0    6 7 3    3 2 6    1 5 0    4 0 5
                    </p>
				</triangles>
			</mesh>
		</geometry>
		<geometry id="child-cube">
			<mesh>
				<source id="child-cube-coordonnees">
					<float_array id="child-cube-coordonnees-donnees" count="24">
                        10 -5 -5   20 -5 -5  10  5 -5   20  5 -5
                        10 -5  5   20 -5  5  10  5  5   20  5  5
                    </float_array>
					<technique_common>
						<accessor count="8" offset="0" source="#child-cube-coordonnees-donnees" stride="3">
							<param name="X" type="float"/>
							<param name="Y" type="float"/>
							<param name="Z" type="float"/>
						</accessor>
					</technique_common>
				</source>
				<vertices id="child-cube-sommets">
					<input semantic="POSITION" source="#child-cube-coordonnees"/>
				</vertices>
				<triangles count="12">
					<input offset="0" semantic="VERTEX" source="#child-cube-sommets"/>
					<p>
                        4 5 1    0 4 1    4 6 5    2 6 4    6 7 5    3 7 6
                        0 3 4    1 5 7    3 0 2    1 7 3    2 0 1    6 2 3
                    </p>
				</triangles>
			</mesh>
		</geometry>
	</library_geometries>
	<library_visual_scenes>
    <visual_scene id="the-scene">
      <node id="parent-cube">
          <instance_geometry url="#parent-cube"/>
          <node id="child-cube">
              <instance_geometry url="#child-cube"/>
          </node>
      </node>
    </visual_scene>
  </library_visual_scenes>
	<scene>
		<node>
			<translate> 0 0 0 </translate>
			<rotate> 0 0 1 0 </rotate>
			<scale> 1 1 1 </scale>
			<instance url="#the-scene"/>
		</node>
		<instance_visual_scene url="#the-scene"/>
	</scene>
</COLLADA>)"sv;

TEST_CASE("Process COLLADA", "[library]")
{
    std::ofstream file("mock.input.dae");
    REQUIRE(file.is_open());
    file << mock_collada;
    file.close();

    auto importer = Assimp::Importer{};
    auto const* scene = importer.ReadFile("mock.input.dae", aiProcess_Triangulate | aiProcess_FlipUVs);

    SECTION("Load COLLADA")
    {
        REQUIRE(scene != nullptr);
        REQUIRE_FALSE(static_cast<bool>(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE));
        REQUIRE(scene->mRootNode != nullptr);
    }

    SECTION("Analyze mesh")
    {
        auto const meshes = std::span{scene->mMeshes, scene->mNumMeshes};
        CHECK(meshes.size() == 2);

        SECTION("Parent cube")
        {
            auto const* parent_cube = meshes.back();
            CHECK(parent_cube->mNumFaces == 12);
            CHECK(parent_cube->mNumVertices == 36);
            CHECK(std::string_view{parent_cube->mName.C_Str()} == "parent-cube");
        }

        SECTION("Child cube")
        {
            auto const* child_cube = meshes.front();
            CHECK(child_cube->mNumFaces == 12);
            CHECK(child_cube->mNumVertices == 36);
            CHECK(std::string_view{child_cube->mName.C_Str()} == "child-cube");
        }
    }

    SECTION("Transformations")
    {
        auto const* parent_node = scene->mRootNode->FindNode("parent-cube");
        REQUIRE(parent_node != nullptr);
        auto const parent_transform = parent_node->mTransformation;
        CHECK(parent_transform.IsIdentity());
        aiVector3D parent_scaling;
        aiQuaternion parent_rotation;
        aiVector3D parent_position;
        parent_transform.Decompose(parent_scaling, parent_rotation, parent_position);

        auto const* child_node = scene->mRootNode->FindNode("child-cube");
        REQUIRE(child_node != nullptr);
        auto const child_transform = child_node->mTransformation;
        CHECK(child_transform.IsIdentity());
        aiVector3D child_scaling;
        aiQuaternion child_rotation;
        aiVector3D child_position;
        child_transform.Decompose(child_scaling, child_rotation, child_position);

        SECTION("Check")
        {
            SECTION("Parent cube")
            {
                CHECK(parent_scaling == aiVector3D{1, 1, 1});
                CHECK(parent_rotation == aiQuaternion{1, 0, 0, 0});
                CHECK(parent_position == aiVector3D{0, 0, 0});
            }

            SECTION("Child cube")
            {
                CHECK(child_scaling == aiVector3D{1, 1, 1});
                CHECK(child_rotation == aiQuaternion{1, 0, 0, 0});
                CHECK(child_position == aiVector3D{0, 0, 0});
            }
        }
    }
}