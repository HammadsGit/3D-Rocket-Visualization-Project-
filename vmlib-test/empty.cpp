#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"


static constexpr float kEps_ = 1e-6f;

using namespace Catch::Matchers;

TEST_CASE("Matrix Multiplication", "[mat44]") {

    SECTION("Multiplication with Identity") {
        Mat44f a = { {
            1.0f, 2.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f,
            9.0f, 10.0f, 11.0f, 12.0f,
            13.0f, 14.0f, 15.0f, 16.0f
        } };

        // call the multiplication operator
        Mat44f identity = kIdentity44f;
        auto result = a * identity;

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                REQUIRE_THAT(result(i, j), WithinAbs(a(i, j), kEps_));
            }
        }
    }

    SECTION("Multiplication of Two Distinct Matrices") {
        Mat44f a = {
            1.0f, 2.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f,
            9.0f, 10.0f, 11.0f, 12.0f,
            13.0f, 14.0f, 15.0f, 16.0f
        };
        Mat44f b = {
            16.0f, 15.0f, 14.0f, 13.0f,
            12.0f, 11.0f, 10.0f, 9.0f,
            8.0f, 7.0f, 6.0f, 5.0f,
            4.0f, 3.0f, 2.0f, 1.0f
        };
        auto result = a * b;

        // Expected result matrix after multiplication
        Mat44f expected = {
            80.0f, 70.0f, 60.0f, 50.0f,
            240.0f, 214.0f, 188.0f, 162.0f,
            400.0f, 358.0f, 316.0f, 274.0f,
            560.0f, 502.0f, 444.0f, 386.0f
        };

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                REQUIRE_THAT(result(i, j), WithinAbs(expected(i, j), kEps_));
            }
        }
    }

}


TEST_CASE("Matrix Transpose", "[mat44]") {

    SECTION ("Transpose of Identity") {

        Mat44f m = {
            1.f, 2.f, 3.f, 4.f,
            5.f, 6.f, 7.f, 8.f,
            9.f, 10.f, 11.f, 12.f,
            13.f, 14.f, 15.f, 16.f
        };
        auto result = transpose(m);

        REQUIRE_THAT(result(0, 0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(result(0, 1), WithinAbs(5.f, kEps_));
        REQUIRE_THAT(result(0, 2), WithinAbs(9.f, kEps_));
        REQUIRE_THAT(result(0, 3), WithinAbs(13.f, kEps_));
        REQUIRE_THAT(result(1, 0), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(6.f, kEps_));
        REQUIRE_THAT(result(1, 2), WithinAbs(10.f, kEps_));
        REQUIRE_THAT(result(1, 3), WithinAbs(14.f, kEps_));
        REQUIRE_THAT(result(2, 0), WithinAbs(3.f, kEps_));
        REQUIRE_THAT(result(2, 1), WithinAbs(7.f, kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(11.f, kEps_));
        REQUIRE_THAT(result(2, 3), WithinAbs(15.f, kEps_));
        REQUIRE_THAT(result(3, 0), WithinAbs(4.f, kEps_));
        REQUIRE_THAT(result(3, 1), WithinAbs(8.f, kEps_));
        REQUIRE_THAT(result(3, 2), WithinAbs(12.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(16.f, kEps_));
	}

    SECTION("Transpose with different values") {
            Mat44f m = {
        2.f, 3.f, 4.f, 5.f,
        6.f, 7.f, 8.f, 9.f,
        10.f, 11.f, 12.f, 13.f,
        14.f, 15.f, 16.f, 17.f
            };
        auto result = transpose(m);

        REQUIRE_THAT(result(0, 0), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(result(0, 1), WithinAbs(6.f, kEps_));
        REQUIRE_THAT(result(0, 2), WithinAbs(10.f, kEps_));
        REQUIRE_THAT(result(0, 3), WithinAbs(14.f, kEps_));
        REQUIRE_THAT(result(1, 0), WithinAbs(3.f, kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(7.f, kEps_));
        REQUIRE_THAT(result(1, 2), WithinAbs(11.f, kEps_));
        REQUIRE_THAT(result(1, 3), WithinAbs(15.f, kEps_));
        REQUIRE_THAT(result(2, 0), WithinAbs(4.f, kEps_));
        REQUIRE_THAT(result(2, 1), WithinAbs(8.f, kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(12.f, kEps_));
        REQUIRE_THAT(result(2, 3), WithinAbs(16.f, kEps_));
        REQUIRE_THAT(result(3, 0), WithinAbs(5.f, kEps_));
        REQUIRE_THAT(result(3, 1), WithinAbs(9.f, kEps_));
        REQUIRE_THAT(result(3, 2), WithinAbs(13.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(17.f, kEps_));
    }
}


TEST_CASE("Matrix X rotaiton", "[mat44]") {

    SECTION("Rotation of Identity - 180 degrees") {
        float angle = 3.1415926f;  // 180 degrees in radians
        auto result = make_rotation_x(angle);

        REQUIRE_THAT(result(0, 0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(result(1, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(result(2, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(1.f, kEps_));
	}

    SECTION("Rotation of 270 degrees") {
        float angle = 3.1415926f * 3 / 2;  // 270 degrees in radians
        auto result = make_rotation_x(angle);

        REQUIRE_THAT(result(0, 0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(result(1, 2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(result(2, 1), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(1.f, kEps_));
    }

}

TEST_CASE("Rotation Y - 90 Degrees", "[mat44]") {
    SECTION("Rotation of 90 degrees") {
        float angle = 3.1415926f / 2;  // 90 degrees in radians
        auto result = make_rotation_y(angle);

        REQUIRE_THAT(result(0, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(result(0, 2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(result(2, 0), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(1.f, kEps_));
    }

    SECTION("ROTATION of 180 degree's", "[mat44]") {
        float angle = 3.1415926f;  // 180 degrees in radians
        auto result = make_rotation_y(angle);

        REQUIRE_THAT(result(0, 0), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(result(2, 0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(1.f, kEps_));
	}
}

TEST_CASE("Rotation Z ") {
    SECTION("Rotation of 35 degrees") {
        float angle = 35.f * 3.1415926f / 180.f;  // 35 degrees in radians
        auto result = make_rotation_z(angle);

        REQUIRE_THAT(result(0, 0), WithinAbs(cos(angle), kEps_));
        REQUIRE_THAT(result(0, 1), WithinAbs(-sin(angle), kEps_));
        REQUIRE_THAT(result(1, 0), WithinAbs(sin(angle), kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(cos(angle), kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(1.f, kEps_));
	}

    SECTION("Rotation of 160 degrees") {
        float angle = 160.f * 3.1415926f / 180.f;  // 160 degrees in radians
        auto result = make_rotation_z(angle);

        REQUIRE_THAT(result(0, 0), WithinAbs(cos(angle), kEps_));
        REQUIRE_THAT(result(0, 1), WithinAbs(-sin(angle), kEps_));
        REQUIRE_THAT(result(1, 0), WithinAbs(sin(angle), kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(cos(angle), kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(1.f, kEps_));
	}

}

TEST_CASE("Scaling", "[mat44]") {
    // Test with uniform scaling
    SECTION("Uniform Scaling") {
        float s = 2.f;
        auto result = make_scaling(s, s, s);

        REQUIRE_THAT(result(0, 0), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(1.f, kEps_));
    }

    // Test with non-uniform scaling
    SECTION("Non-Uniform Scaling") {
        float sx = 2.f, sy = 3.f, sz = 4.f;
        auto result = make_scaling(sx, sy, sz);

        REQUIRE_THAT(result(0, 0), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(3.f, kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(4.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(1.f, kEps_));
    }
}


TEST_CASE("Translation", "[mat44]") {
    // Test with a specific translation vector
    SECTION("Specific Translation") {
        Vec3f translation = { 1.f, 2.f, 3.f };
        auto result = make_translation(translation);

        REQUIRE_THAT(result(0, 3), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(result(1, 3), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(result(2, 3), WithinAbs(3.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(1.f, kEps_));
    }

    // Test with zero translation vector
    SECTION("Zero Translation") {
        Vec3f translation = { 0.f, 0.f, 0.f };
        auto result = make_translation(translation);

        REQUIRE_THAT(result(0, 3), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(result(1, 3), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(result(2, 3), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(result(3, 3), WithinAbs(1.f, kEps_));
    }
}

TEST_CASE("Perspective Projection", "[mat44]") {

    // Test with standard FOV and aspect ratio
    SECTION("Standard Projection") {
        float fov = 60.f * 3.1415926f / 180.f; // 60 degrees in radians
        float aspect = 1280 / float(720);
        float near = 0.1f, far = 100.f;
        auto result = make_perspective_projection(fov, aspect, near, far);

        REQUIRE_THAT(result(0, 0), WithinAbs(1 / tan(fov / 2) / aspect, kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(1 / tan(fov / 2), kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(-(far + near) / (far - near), kEps_));
        REQUIRE_THAT(result(2, 3), WithinAbs(-2 * far * near / (far - near), kEps_));
        REQUIRE_THAT(result(3, 2), WithinAbs(-1.f, kEps_));
    }

    // Test with a different FOV and aspect ratio
    SECTION("Different Projection") {
        float fov = 45.f * 3.1415926f / 180.f; // 45 degrees in radians
        float aspect = 1920 / float(1080);
        float near = 0.5f, far = 50.f;
        auto result = make_perspective_projection(fov, aspect, near, far);

        REQUIRE_THAT(result(0, 0), WithinAbs(1 / tan(fov / 2) / aspect, kEps_));
        REQUIRE_THAT(result(1, 1), WithinAbs(1 / tan(fov / 2), kEps_));
        REQUIRE_THAT(result(2, 2), WithinAbs(-(far + near) / (far - near), kEps_));
        REQUIRE_THAT(result(2, 3), WithinAbs(-2 * far * near / (far - near), kEps_));
        REQUIRE_THAT(result(3, 2), WithinAbs(-1.f, kEps_));
    }
}



int main(int argc, char* argv[])
{
	return Catch::Session().run(argc, argv);
}