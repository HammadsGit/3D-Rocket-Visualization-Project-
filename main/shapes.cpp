#include "shapes.hpp"

#include "../vmlib/mat33.hpp"

SimpleMeshData make_cylinder( std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{
	std::vector<Vec3f> pos;
	std::vector<Vec3f> normals;
    // Compute the normal matrix
    Mat33f const N = mat44_to_mat33(transpose(invert(aPreTransform)));

    float prevY = std::cos(0.f);
    float prevZ = std::sin(0.f);

    for (std::size_t i = 0; i < aSubdivs; ++i) {
        float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
        float y = std::cos(angle);
        float z = std::sin(angle);

        // Positions
        pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
        pos.emplace_back(Vec3f{ 0.f, y, z });
        pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });

        normals.emplace_back(Vec3f{ 0.f, prevY, prevZ });
        normals.emplace_back(Vec3f{ 0.f, y, z });
        normals.emplace_back(Vec3f{ 1.f, prevY, prevZ });

        pos.emplace_back(Vec3f{ 0.f, y, z });
        pos.emplace_back(Vec3f{ 1.f, y, z });
        pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });

        normals.emplace_back(Vec3f{ 0.f, y, z });
        normals.emplace_back(Vec3f{ 1.f, y, z });
        normals.emplace_back(Vec3f{ 1.f, prevY, prevZ });


        prevY = y;
        prevZ = z;
    }

    // Normals for the caps
    
        Vec3f normalCapNegX = Vec3f{ -1.f, 0.f, 0.f }; // Normal for cap at x = 0
        Vec3f normalCapPosX = Vec3f{ 1.f, 0.f, 0.f };  // Normal for cap at x = 1
        for (std::size_t i = 0; i < aSubdivs; ++i) {
            float const angle = i * 2.f * 3.1415926f / aSubdivs;
            float nextAngle = (i + 1) * 2.f * 3.1415926f / aSubdivs;

            float y = std::cos(angle);
            float z = std::sin(angle);
            float nextY = std::cos(nextAngle);
            float nextZ = std::sin(nextAngle);

            // Cap at x = 0
            pos.emplace_back(Vec3f{ 0.f, 0.f, 0.f });
            pos.emplace_back(Vec3f{ 0.f, y, z });
            pos.emplace_back(Vec3f{ 0.f, nextY, nextZ });

            normals.emplace_back(Vec3f{ 0.f, 0.f, 0.f });
            normals.emplace_back(Vec3f{ 0.f, y, z });
            normals.emplace_back(Vec3f{ 0.f, nextY, nextZ });

            // Cap at x = 1
            pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
            pos.emplace_back(Vec3f{ 1.f, nextY, nextZ });
            pos.emplace_back(Vec3f{ 1.f, y, z });

            normals.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
            normals.emplace_back(Vec3f{ 1.f, nextY, nextZ });
            normals.emplace_back(Vec3f{ 1.f, y, z });


        }
    


	for (auto& p : pos) {
		Vec4f p4{ p.x, p.y, p.z, 1.0f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;

		Vec3f b = Vec3f{ t.x, t.y, t.z };
		p = b;
	}

	for (auto& n : normals) {
		Vec3f p3{ n.x, n.y, n.z };
		Vec3f t = N * p3;

		Vec3f b = Vec3f{ t.x, t.y, t.z };
		n = b;
	}


	Vec3f color = aColor;
	std::vector col(pos.size(), color);
	std::vector texcoords(pos.size(), Vec2f{ 1.f, 1.f });
	return SimpleMeshData{ std::move(pos), std::move(col) , std::move(normals)};
}

SimpleMeshData make_cube( Vec3f aColor, Mat44f aPreTransform) {
    std::vector<Vec3f> pos;
    std::vector<Vec3f> normals;

    // Front face (Z+)
    pos.push_back({ -0.5f, -0.5f, 0.5f });
    pos.push_back({ 0.5f, -0.5f, 0.5f });
    pos.push_back({ 0.5f, 0.5f, 0.5f });
    pos.push_back({ 0.5f, 0.5f, 0.5f });
    pos.push_back({ -0.5f, 0.5f, 0.5f });
    pos.push_back({ -0.5f, -0.5f, 0.5f });

    normals.push_back({ -0.5f, -0.5f, 0.5f });
    normals.push_back({ 0.5f, -0.5f, 0.5f });
    normals.push_back({ 0.5f, 0.5f, 0.5f });
    normals.push_back({ 0.5f, 0.5f, 0.5f });
    normals.push_back({ -0.5f, 0.5f, 0.5f });
    normals.push_back({ -0.5f, -0.5f, 0.5f });

    // Back face (Z-)
    pos.push_back({ -0.5f, -0.5f, -0.5f });
    pos.push_back({ 0.5f, 0.5f, -0.5f });
    pos.push_back({ 0.5f, -0.5f, -0.5f });
    pos.push_back({ 0.5f, 0.5f, -0.5f });
    pos.push_back({ -0.5f, -0.5f, -0.5f });
    pos.push_back({ -0.5f, 0.5f, -0.5f });

    // Back face (Z-)
    normals.push_back({ -0.5f, -0.5f, -0.5f });
    normals.push_back({ 0.5f, 0.5f, -0.5f });
    normals.push_back({ 0.5f, -0.5f, -0.5f });
    normals.push_back({ 0.5f, 0.5f, -0.5f });
    normals.push_back({ -0.5f, -0.5f, -0.5f });
    normals.push_back({ -0.5f, 0.5f, -0.5f });


    // Left face (X-)
    pos.push_back({ -0.5f, -0.5f, -0.5f });
    pos.push_back({ -0.5f, -0.5f, 0.5f });
    pos.push_back({ -0.5f, 0.5f, 0.5f });
    pos.push_back({ -0.5f, 0.5f, 0.5f });
    pos.push_back({ -0.5f, 0.5f, -0.5f });
    pos.push_back({ -0.5f, -0.5f, -0.5f });

    // Left face (X-)
    normals.push_back({ -0.5f, -0.5f, -0.5f });
    normals.push_back({ -0.5f, -0.5f, 0.5f });
    normals.push_back({ -0.5f, 0.5f, 0.5f });
    normals.push_back({ -0.5f, 0.5f, 0.5f });
    normals.push_back({ -0.5f, 0.5f, -0.5f });
    normals.push_back({ -0.5f, -0.5f, -0.5f });

    // Right face (X+)
    pos.push_back({ 0.5f, -0.5f, -0.5f });
    pos.push_back({ 0.5f, 0.5f, 0.5f });
    pos.push_back({ 0.5f, -0.5f, 0.5f });
    pos.push_back({ 0.5f, 0.5f, 0.5f });
    pos.push_back({ 0.5f, -0.5f, -0.5f });
    pos.push_back({ 0.5f, 0.5f, -0.5f });

    normals.push_back({ 0.5f, -0.5f, -0.5f });
    normals.push_back({ 0.5f, 0.5f, 0.5f });
    normals.push_back({ 0.5f, -0.5f, 0.5f });
    normals.push_back({ 0.5f, 0.5f, 0.5f });
    normals.push_back({ 0.5f, -0.5f, -0.5f });
    normals.push_back({ 0.5f, 0.5f, -0.5f });


    // Top face (Y+)
    pos.push_back({ -0.5f, 0.5f, -0.5f });
    pos.push_back({ 0.5f, 0.5f, 0.5f });
    pos.push_back({ 0.5f, 0.5f, -0.5f });
    pos.push_back({ 0.5f, 0.5f, 0.5f });
    pos.push_back({ -0.5f, 0.5f, -0.5f });
    pos.push_back({ -0.5f, 0.5f, 0.5f });

    // Top face (Y+)
    normals.push_back({ -0.5f, 0.5f, -0.5f });
    normals.push_back({ 0.5f, 0.5f, 0.5f });
    normals.push_back({ 0.5f, 0.5f, -0.5f });
    normals.push_back({ 0.5f, 0.5f, 0.5f });
    normals.push_back({ -0.5f, 0.5f, -0.5f });
    normals.push_back({ -0.5f, 0.5f, 0.5f });


    // Bottom face (Y-)
    pos.push_back({ -0.5f, -0.5f, -0.5f });
    pos.push_back({ 0.5f, -0.5f, 0.5f });
    pos.push_back({ 0.5f, -0.5f, -0.5f });
    pos.push_back({ 0.5f, -0.5f, 0.5f });
    pos.push_back({ -0.5f, -0.5f, -0.5f });
    pos.push_back({ -0.5f, -0.5f, 0.5f });

    // Bottom face (Y-)
    normals.push_back({ -0.5f, -0.5f, -0.5f });
    normals.push_back({ 0.5f, -0.5f, 0.5f });
    normals.push_back({ 0.5f, -0.5f, -0.5f });
    normals.push_back({ 0.5f, -0.5f, 0.5f });
    normals.push_back({ -0.5f, -0.5f, -0.5f });
    normals.push_back({ -0.5f, -0.5f, 0.5f });




    // Apply the pre-transform to all positions and transform normals

    Mat33f N = mat44_to_mat33(transpose(invert(aPreTransform)));
    for (auto& p : pos) {
        Vec4f p4{ p.x, p.y, p.z, 1.f };
        Vec4f t = aPreTransform * p4;
        p = { t.x, t.y, t.z };
    }

    for (auto& n : normals) {
        Vec3f p3{ n.x, n.y, n.z };
        Vec3f t = N * p3;

        Vec3f b = Vec3f{ t.x, t.y, t.z };
        n = b;
    }

    std::vector<Vec3f> col(pos.size(), aColor);
    return SimpleMeshData{ std::move(pos), std::move(col), std::move(normals) };
}

Vec3f cross(const Vec3f& v1, const Vec3f& v2) {
    return Vec3f{
        v1.y * v2.z - v1.z * v2.y,  // X component
        v1.z * v2.x - v1.x * v2.z,  // Y component
        v1.x * v2.y - v1.y * v2.x   // Z component
    };
}

SimpleMeshData make_rectangle(Vec3f aColor) {

	std::vector<Vec3f> positions = {
		//left triangle
		Vec3f { -1, 1, 0},
		Vec3f {-1, -1, 0},
		Vec3f {1, -1, 0},

		//right triangle
		Vec3f { -1, 1, 0},
		Vec3f {1, -1, 0},
		Vec3f {1, 1, 0},
	};

    std::vector<Vec3f> col(positions.size(), aColor);
    return SimpleMeshData{ std::move(positions), std::move(col)};
}


SimpleMeshData make_pyramid( Vec3f aColor, Mat44f aPreTransform) {


    {

        Mat33f const N = mat44_to_mat33(transpose(invert(aPreTransform)));
        std::vector<Vec3f> pos;
        std::vector<Vec3f> normals;

        // Front face (Z+)


        float pyramidYOffset = 0.5f; // You can adjust this value as needed

        // Front face (Z+)
        pos.push_back({ -0.5f, -0.5f + pyramidYOffset, -0.5f });
        pos.push_back({ 0.5f, -0.5f + pyramidYOffset, -0.5f });
        pos.push_back({ 0.0f, 0.5f + pyramidYOffset, 0.0f });

        // Back face (Z-)
        pos.push_back({ -0.5f, -0.5f + pyramidYOffset, 0.5f });
        pos.push_back({ 0.5f, -0.5f + pyramidYOffset, 0.5f });
        pos.push_back({ 0.0f, 0.5f + pyramidYOffset, 0.0f });

        // Left face (X-)
        pos.push_back({ -0.5f, -0.5f + pyramidYOffset, -0.5f });
        pos.push_back({ -0.5f, -0.5f + pyramidYOffset, 0.5f });
        pos.push_back({ 0.0f, 0.5f + pyramidYOffset, 0.0f });

        // Right face (X+)
        pos.push_back({ 0.5f, -0.5f + pyramidYOffset, -0.5f });
        pos.push_back({ 0.5f, -0.5f + pyramidYOffset, 0.5f });
        pos.push_back({ 0.0f, 0.5f + pyramidYOffset, 0.0f });


        // Define normals for each face
        normals.push_back({ -0.5f, -0.5f + pyramidYOffset, -0.5f });
        normals.push_back({ 0.5f, -0.5f + pyramidYOffset, -0.5f });
        normals.push_back({ 0.0f, 0.5f + pyramidYOffset, 0.0f });

        // Back face (Z-)
        normals.push_back({ -0.5f, -0.5f + pyramidYOffset, 0.5f });
        normals.push_back({ 0.5f, -0.5f + pyramidYOffset, 0.5f });
        normals.push_back({ 0.0f, 0.5f + pyramidYOffset, 0.0f });

        // Left face (X-)
        normals.push_back({ -0.5f, -0.5f + pyramidYOffset, -0.5f });
        normals.push_back({ -0.5f, -0.5f + pyramidYOffset, 0.5f });
        normals.push_back({ 0.0f, 0.5f + pyramidYOffset, 0.0f });

        // Right face (X+)
        normals.push_back({ 0.5f, -0.5f + pyramidYOffset, -0.5f });
        normals.push_back({ 0.5f, -0.5f + pyramidYOffset, 0.5f });
        normals.push_back({ 0.0f, 0.5f + pyramidYOffset, 0.0f });


        //apply the pre-transform to all positions and transform normals



        for (auto& p : pos) {
            Vec4f p4{ p.x, p.y, p.z, 1.f };
            Vec4f t = aPreTransform * p4;
            p = { t.x, t.y, t.z };
        }

        for (auto& n : normals) {
            Vec3f p3{ n.x, n.y, n.z };
            Vec3f t = N * p3;

            Vec3f b = Vec3f{ t.x, t.y, t.z };
            n = b;
        }

        std::vector<Vec3f> col(pos.size(), aColor);
        return SimpleMeshData{ std::move(pos), std::move(col), std::move(normals) };
    }
}

