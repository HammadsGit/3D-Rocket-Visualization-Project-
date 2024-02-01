#include "loadobj.hpp"
#include <rapidobj/rapidobj.hpp>
#include "../support/error.hpp"

SimpleMeshData load_wavefront_obj(char const* aPath)
{
	auto result = rapidobj::ParseFile(aPath);
	if (result.error) {
		throw Error("Unable to load OBJ file '%s': %s", aPath, result.error.code.message().c_str());
	}

	//obj files can hold non triangle shapes but opengl only renders triangles so we have to convert them
	rapidobj::Triangulate(result);

	SimpleMeshData ret;
	for (auto const& shape : result.shapes) {
		for (std::size_t i = 0; i < shape.mesh.indices.size(); i++) {
			auto const& idx = shape.mesh.indices[i];

			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2],
				});
			//get normals
			ret.normals.emplace_back(Vec3f{
			   result.attributes.normals[idx.normal_index * 3 + 0],
			   result.attributes.normals[idx.normal_index * 3 + 1],
			   result.attributes.normals[idx.normal_index * 3 + 2],
				});

			ret.texcoords.emplace_back(Vec2f{
				result.attributes.texcoords[idx.texcoord_index * 2 + 0],
				result.attributes.texcoords[idx.texcoord_index * 2 + 1],
				});

			//as its triangles, we can find the face index by dividing the vertex index by three
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];


			ret.colors.emplace_back(Vec3f{
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2],
				});

		}



	}
	return ret;
}

