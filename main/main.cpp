#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"

#include "defaults.hpp"
#include "simple_mesh.hpp"
#include "loadobj.hpp"
#include "../vmlib/mat33.hpp"
#include "textures.hpp"
#include "shapes.hpp"
#include "particle.hpp"


namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - CW2";
	
	constexpr float kPi_ = 3.1415926f;

	constexpr float kMouseSensitivity_ = 0.01f; // radians per pixel

	struct CameraValues
	{
		Vec3f cameraFront;
		Vec3f cameraUp;
		Vec3f cameraPos;
		float movementSpeed;
		float yaw;
		float pitch;
		bool firstMouse;
	};

	struct State_
	{
		ShaderProgram* prog;

		struct CamCtrl_
		{
			bool cameraActive;
			bool actionZoomIn, actionZoomOut;
			bool moveForward, moveBackward, moveUp, moveDown;
			bool speedUp, speedDown;
			bool moveLeft, moveRight;
			bool animationActive;

			float phi, theta;
			float radius;
			float x_move_speed, y_move_speed, z_move_speed;

			float lastX, lastY;
			float currentX, currentY;
		} camControl, camComtrolViewport2;

		std::vector<Vec3f> lightPositions;
		std::vector<Vec3f> lightColors;
		std::vector<Vec3f> vertPositions;

		bool mouseLeftPressed;
		float mousePressedX;
		float mousePressedY;

		bool splitscreen;
		bool switchscreen;
		enum cameraTracking
		{
			cameraNormal,
			cameraTrackGround,
			cameraTrackFollow
		} cameraMode = cameraNormal, cameraMode2 = cameraNormal;

		
		Vec3f vecSpaceshipTranslation;
	};

	
	void glfw_callback_error_( int, char const* );

	void glfw_callback_key_( GLFWwindow*, int, int, int, int );

	void glfw_callback_mouseclick_(GLFWwindow* aWindow, int button, int action, int mods);

	void glfw_callback_motion_( GLFWwindow*, double, double);

	float radians(float degrees);
	
	void draw_land_mass(GLuint shaderId, Mat44f projCameraWorld, Mat33f normalMatrix, Vec3f lightDir, GLuint tex, GLuint vao, int vertexCount);

	void draw_landing_pad(GLuint shaderId, Mat44f projection, Mat44f lookAt, Mat44f translations[], int numTranslations,
		Mat33f normalMatrix, Vec3f lightDir, GLuint vao, int vertexCount,
		std::vector<Vec3f> lightPositions, std::vector<Vec3f> lightColors, std::vector<Vec3f> vertPositions);

	void draw_spaceship(GLuint shaderId, Mat44f projection, Mat44f lookAt, Mat44f translation, Mat33f normalMatrix, Vec3f lightDir, GLuint vao, int vertexCount,
		std::vector<Vec3f> lightPositions, std::vector<Vec3f> lightColors, std::vector<Vec3f> vertPositions);

	void draw_particles(GLuint shaderId, Mat44f projection, Mat44f lookAt, Mat44f translation, Mat33f normalMatrix,
		std::vector<Particle> particleList, std::vector<GLuint> vao, int vertexCount, std::vector<Vec3f> particleMovement);

	Mat44f lookAt(Vec3f eye, Vec3f target);

	CameraValues get_camera_values(State_ &state, float movementSpeed, float dt, Vec3f cameraPos, Vec3f cameraFront, Vec3f cameraUp,
			float xDiff, float yDiff, float yaw, float pitch, bool firstMouse);

	std::vector<Vec3f> get_lightpositions();
	std::vector<Vec3f> get_lightcolors();
	std::vector<Vec3f> get_vertpositions();

	GLuint create_rectangle_vao(std::vector<Vec2f> kPositions, Vec4f color);

	Vec2f translate_2d_to_xy(float x_2d, float y_2d, float screenWidth, float screenHeight);

	bool is_mouse_in_area(float mouse_x, float mouse_y, std::vector<Vec2f> boundingCoords);

	std::vector<Vec2f> generate_outlines(std::vector<Vec2f> rectangle);


	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};
}

int main() try
{

	// Initialize GLFW
	if( GLFW_TRUE != glfwInit() )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwInit() failed with '%s' (%d)", msg, ecode );
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_ );

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint( GLFW_DEPTH_BITS, 24 );

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if( !window )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwCreateWindow() failed with '%s' (%d)", msg, ecode );
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	// TODO: Additional event handling setup
	State_ state{};
	glfwSetWindowUserPointer(window, &state);

	glfwSetKeyCallback( window, &glfw_callback_key_ );
	glfwSetCursorPosCallback(window, &glfw_callback_motion_);
	glfwSetMouseButtonCallback(window, &glfw_callback_mouseclick_);


	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if( !gladLoadGLLoader( (GLADloadproc)&glfwGetProcAddress ) )
		throw Error( "gladLoaDGLLoader() failed - cannot load GL API!" );

	std::printf( "RENDERER %s\n", glGetString( GL_RENDERER ) );
	std::printf( "VENDOR %s\n", glGetString( GL_VENDOR ) );
	std::printf( "VERSION %s\n", glGetString( GL_VERSION ) );
	std::printf( "SHADING_LANGUAGE_VERSION %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();
	//-----------------------------------------------------------------------
	// TODO: global GL setup goes here
	glEnable(GL_FRAMEBUFFER_SRGB);
	//glEnable(GL_CULL_FACE);
	glClearColor(0.2f, 0.2f, 0.2f, 0.f);
	glEnable(GL_DEPTH_TEST);


	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	// This can be different from the window size, as standard window
	// decorations (title bar, borders, ...) may be included in the window size
	// but not be part of the drawable surface area.
	int iwidth, iheight;
	glfwGetFramebufferSize( window, &iwidth, &iheight );

	glViewport( 0, 0, iwidth, iheight );

	// Load shader program
	ShaderProgram prog({
		{ GL_VERTEX_SHADER, "assets/default.vert" },
		{ GL_FRAGMENT_SHADER, "assets/default.frag" }
	});

	ShaderProgram colorShader({
		{ GL_VERTEX_SHADER, "assets/colorShader.vert" },
		{ GL_FRAGMENT_SHADER, "assets/colorShader.frag" }
	});

	ShaderProgram particleShader({
		{ GL_VERTEX_SHADER, "assets/particleShader.vert" },
		{ GL_FRAGMENT_SHADER, "assets/particleShader.frag" }
	});

	ShaderProgram uiShader({
		{ GL_VERTEX_SHADER, "assets/uiShader.vert" },
		{ GL_FRAGMENT_SHADER, "assets/uiShader.frag" }
	});

	state.prog = &prog;
	state.camControl.radius = 10.f;
	state.camControl.x_move_speed = 0.f;
	state.camControl.y_move_speed = 0.f;
	state.camControl.z_move_speed = 0.f;

	state.camControl.currentX = float(iwidth) / 2;
	state.camControl.currentY = float(iheight) / 2;

	// Animation state
	auto last = Clock::now();
	// CREATE OBJECTS --------------------------------------------------------------------------------------------------------------------
	//set up the the landmass 
	auto mesh = load_wavefront_obj("assets/parlahti.obj");
	auto tex = load_texture_2d("assets/L4343A-4k.jpeg");
	auto mesh_vertex_count = mesh.positions.size();
	GLuint vao = create_vao(mesh);

	//set up landingpad 
	auto landingpad = load_wavefront_obj("assets/landingpad.obj");
	auto landingpad_vertex_count = landingpad.positions.size();
	GLuint landingpad_vao = create_vao(landingpad);

	//make spaceship
	auto cuboid = make_cube({ 0.2f, 0.20f, 0.20f },make_translation({ 0.0f, 1.75f, 0.0f }) * make_scaling(0.5f, 3.0f, 0.5f));
	auto pyramid = make_pyramid( { 0.2f, 0.20f, 0.20f }, make_translation({ 0.0f, 3.25, 0.0f }) * make_rotation_x(0.0f) * make_scaling(0.5f, 1.0f, 0.5f));
	auto cylinder = make_cylinder(128, { 0.2f, 0.2f, 0.2f }, make_translation({ 0.0f, 0.0f, 0.0f }) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.25f, 0.25f, 0.25f));
	auto thruster1 = make_cylinder(128, { 0.2f, 0.2f, 0.2f }, make_translation({ 0.0f, 0.2f, 0.35f }) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.5f, 0.12f, 0.12f));
	auto thrusterCap1 = make_pyramid( { 0.2f, 0.20f, 0.20f }, make_translation({ 0.0f, 0.60f, 0.25f }) * make_rotation_x(0.0f) * make_scaling(0.5f, 1.0f, 0.5f));
	auto thruster2 = make_cylinder(128, { 0.2f, 0.2f, 0.2f }, make_translation({ 0.0f, 0.2f, -0.35f }) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.5f, 0.12f, 0.12f));
	auto thrusterCap2 = make_pyramid( { 0.2f, 0.20f, 0.20f }, make_translation({ 0.0f, 0.60f, -0.25f }) * make_rotation_x(0.0f) * make_scaling(0.5f, 1.0f, 0.5f));
	auto thruster3 = make_cylinder(128, { 0.2f, 0.2f, 0.2f }, make_translation({ 0.35f, 0.2f, 0.f }) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.5f, 0.12f, 0.12f));
	auto thrusterCap3 = make_pyramid( { 0.2f, 0.20f, 0.20f }, make_translation({ 0.25f, 0.60f, 0.f }) * make_rotation_x(0.0f) * make_scaling(0.5f, 1.0f, 0.5f));
	auto thruster4 = make_cylinder(128, { 0.2f, 0.2f, 0.2f }, make_translation({ -0.35f, 0.2f, 0.f }) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.5f, 0.12f, 0.12f));
	auto thrusterCap4 = make_pyramid( { 0.2f, 0.2f, 0.2f }, make_translation({ -0.25f, 0.60f, 0.f }) * make_rotation_x(0.0f) * make_scaling(0.5f, 1.0f, 0.5f));

	// concatenate
	auto spaceship_top = concatenate(std::move(cuboid), std::move(pyramid));
	auto spaceship_bottom = concatenate(std::move(cylinder), std::move(thruster1));
	spaceship_bottom = concatenate(std::move(spaceship_bottom), std::move(thrusterCap1));
	spaceship_bottom = concatenate(std::move(spaceship_bottom), std::move(thruster2));
	spaceship_bottom = concatenate(std::move(spaceship_bottom), std::move(thrusterCap2));
	spaceship_bottom = concatenate(std::move(spaceship_bottom), std::move(thruster3));
	spaceship_bottom = concatenate(std::move(spaceship_bottom), std::move(thrusterCap3));
	spaceship_bottom = concatenate(std::move(spaceship_bottom), std::move(thruster4));
	spaceship_bottom = concatenate(std::move(spaceship_bottom), std::move(thrusterCap4));
	auto spaceship = concatenate(std::move(spaceship_bottom), std::move(spaceship_top));

	auto spaceship_vertex_count = spaceship.positions.size();
	GLuint spaceship_vao = create_vao(spaceship);

	//get light values for spacehip + landingpad
	state.lightPositions = get_lightpositions();
	state.lightColors = get_lightcolors();
	state.vertPositions = get_vertpositions();
	std::vector<Particle> listofParticles;
	
	std::vector<GLuint> particleVAOList;
	std::vector<Vec3f> particleMovement;
	unsigned int noOfParticles = 80;
	
	//create particles
	
	for (unsigned int i = 0; i < noOfParticles; ++i) {

		float x_acc = static_cast<float>(0.1 * random(-4, 4));
		float y_acc = static_cast<float>(0.1 * random(-5, 0));
		float z = static_cast<float>(0.1 * random(-4, 4));
		Vec3f translation = { x_acc,y_acc,z };
		particleMovement.push_back( translation);

	}

	for (unsigned int i = 0; i < noOfParticles; ++i) {
		//create one particles
		Particle particles = createParticles(make_translation(particleMovement[i])*make_scaling(0.05f, 0.05f, 0.05f));

		GLuint getvao = create_vao(particles.particledraw);
		particleVAOList.push_back(getvao);
		listofParticles.push_back(particles);
	
	}
	auto particle_vertexCount = static_cast<int>(listofParticles[0].particledraw.positions.size());


	//FOR LOOK AT
	Vec3f cameraPos = { 0.f, 0.f, 10.f };
	Vec3f cameraFront = { 0.f, 0.f, -1.f };
	Vec3f cameraUp = { 0.f, 1.f, 0.f };

	Vec3f cameraPos2 = { 0.f, 0.f, 10.f };
	Vec3f cameraFront2 = { 0.f, 0.f, -1.f };
	Vec3f cameraUp2 = { 0.f, 1.f, 0.f };

	float movementSpeed = 5.f;
	float movementSpeed2 = 5.f;

	//default value of -90 as normally it points down x axis but we want to point down z axis
	float yaw = -90.f;
	float pitch = 0.f;
	float yaw2 = -90.f;
	float pitch2 = 0.f;

	//so we dont do big jump initially in camera position
	bool firstMouse = true;
	bool firstMouse2 = true;

	float animation_time = 0.f; 
	Mat44f spaceship_flight = make_translation(Vec3f{ 0.f, 0.f, 0.f });

	const int numLandingPads = 2;
	Vec3f secondLandingpadTranslation = Vec3f{ 15.f, -0.95f, -10.f };
	Mat44f landingPadTranslation[numLandingPads] = { make_translation(Vec3f{-20.f, -0.95f, -30.f}),
									 make_translation(secondLandingpadTranslation) };
	//because the spaceship is also same translation as second landing pad
	state.vecSpaceshipTranslation = secondLandingpadTranslation;

	//MAKE RECTANGLE FOR UI
	std::vector<Vec2f> buttonReset{};
	buttonReset.push_back(Vec2f{ 0.1f, -0.7f }); //tl
	buttonReset.push_back(Vec2f{ 0.1f, -0.8f }); //bl
	buttonReset.push_back(Vec2f{ 0.3f, -0.8f }); //br

	buttonReset.push_back(Vec2f{ 0.1f, -0.7f });
	buttonReset.push_back(Vec2f{ 0.3f, -0.7f }); //tr
	buttonReset.push_back(Vec2f{ 0.3f, -0.8f });

	std::vector<Vec2f> buttonResetOutlines = generate_outlines(buttonReset);

	std::vector<Vec2f> resetButtonBoundingBox{};

	std::vector<Vec2f> buttonLaunch{};
	buttonLaunch.push_back(Vec2f{ -0.3f, -0.7f }); //tl
	buttonLaunch.push_back(Vec2f{ -0.3f, -0.8f }); //bl
	buttonLaunch.push_back(Vec2f{ -0.1f, -0.8f }); //br

	buttonLaunch.push_back(Vec2f{ -0.3f, -0.7f });
	buttonLaunch.push_back(Vec2f{ -0.1f, -0.7f }); //tr
	buttonLaunch.push_back(Vec2f{ -0.1f, -0.8f });

	std::vector<Vec2f> outlines = generate_outlines(buttonLaunch);

	std::vector<Vec2f> launchButtonBoundingBox{};
	//we need to put this in main loop eventually to handle change of screen size


	auto launchButtonOutlineVao = create_rectangle_vao(outlines, Vec4f{0.f, 1.f, 0.f, 1.f});
	auto launchButtonVao = create_rectangle_vao(buttonLaunch, Vec4f{1.f, 0.f, 0.f, 0.5f});

	auto resetButtonOutlineVao = create_rectangle_vao(buttonResetOutlines, Vec4f{0.f, 1.f, 0.f, 1.f});
	auto resetButtonVao = create_rectangle_vao(buttonReset, Vec4f{0.f, 0.f, 1.f, 0.5f});

	// create query struct
	struct QueryPerformance {
		GLuint startQuery, endQuery;
		GLuint64 duration = 0;
		bool resultReady = false;
		std::string name;

		QueryPerformance() : startQuery(0), endQuery(0), duration(0), resultReady(false), name("") {};

	};

	//create query objects
	QueryPerformance FullRender, basicRendering, instancing, customModel, view1, view2;

	// initialise query for full rendering.
	GLuint startFrameQuery, endFrameQuery;
	glGenQueries(1, &startFrameQuery);
	glGenQueries(1, &endFrameQuery);

	//Initialize timer query for rendering basics.
	GLuint startBasicQuery, endBasicQuery;
	glGenQueries(1, &startBasicQuery);
	glGenQueries(1, &endBasicQuery);

	// Initialize timer query for Instancing
	GLuint startInstancingQuery, endInstancingQuery;
	glGenQueries(1, &startInstancingQuery);
	glGenQueries(1, &endInstancingQuery);

	// Initialize timer query for custom model
	GLuint startCustomModelQuery, endCustomModelQuery;
	glGenQueries(1, &startCustomModelQuery);
	glGenQueries(1, &endCustomModelQuery);

	// Initialize timer query for viewports
	GLuint startView1Query, endView1Query, startView2Query, endView2Query;
	glGenQueries(1, &startView1Query);
	glGenQueries(1, &endView1Query);
	glGenQueries(1, &startView2Query);
	glGenQueries(1, &endView2Query);
	int view1Time = 0;

	//start frame to frame variable.
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - last);

	while( !glfwWindowShouldClose( window ) )
	{
		// Start full frame query
		glQueryCounter(startFrameQuery, GL_TIMESTAMP);

		//start timer.
		auto start = std::chrono::high_resolution_clock::now();

		// Let GLFW process events
		glfwPollEvents();
		
		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize( window, &nwidth, &nheight );

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if( 0 == nwidth || 0 == nheight )
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize( window, &nwidth, &nheight );
				} while( 0 == nwidth || 0 == nheight );
			}

			if (state.splitscreen) {
				//the other view port that makes up the splitscreen is at the bottom of the main loop 
				glViewport( 0, 0, static_cast<GLsizei>(nwidth / 2.f), static_cast<GLsizei>(nheight));
			}
			else {
				glViewport( 0, 0, nwidth, nheight );
			}
		}

		//we need to put this in main loop eventually to handle change of screen size
		Vec2f tl = translate_2d_to_xy(buttonReset[0].x, buttonReset[0].y, fbwidth, fbheight);
		Vec2f tr = translate_2d_to_xy(buttonReset[4].x, buttonReset[4].y, fbwidth, fbheight);
		Vec2f bl = translate_2d_to_xy(buttonReset[1].x, buttonReset[1].y, fbwidth, fbheight);
		Vec2f br = translate_2d_to_xy(buttonReset[2].x, buttonReset[2].y, fbwidth, fbheight);

		resetButtonBoundingBox.clear();
		resetButtonBoundingBox.push_back(tl);
		resetButtonBoundingBox.push_back(tr);
		resetButtonBoundingBox.push_back(bl);
		resetButtonBoundingBox.push_back(br);

		tl = translate_2d_to_xy(buttonLaunch[0].x, buttonLaunch[0].y, fbwidth, fbheight);
		tr = translate_2d_to_xy(buttonLaunch[4].x, buttonLaunch[4].y, fbwidth, fbheight);
		bl = translate_2d_to_xy(buttonLaunch[1].x, buttonLaunch[1].y, fbwidth, fbheight);
		br = translate_2d_to_xy(buttonLaunch[2].x, buttonLaunch[2].y, fbwidth, fbheight);

		launchButtonBoundingBox.clear();
		launchButtonBoundingBox.push_back(tl);
		launchButtonBoundingBox.push_back(tr);
		launchButtonBoundingBox.push_back(bl);
		launchButtonBoundingBox.push_back(br);

		auto const now = Clock::now();
		//delta time means speed can be framerate independant
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;

		float xDiff = state.camControl.currentX - state.camControl.lastX;
		float yDiff = state.camControl.lastY - state.camControl.currentY;


		state.camControl.lastX = state.camControl.currentX;
		state.camControl.lastY = state.camControl.currentY;

		bool inLaunchBtnArea = is_mouse_in_area(state.camControl.currentX, state.camControl.currentY, launchButtonBoundingBox);
		bool inResetBtnArea = is_mouse_in_area(state.camControl.currentX, state.camControl.currentY, resetButtonBoundingBox);

		//two camera values due to splitscreen 
		if (!state.switchscreen) {
			auto cameraVals = get_camera_values(state, movementSpeed, dt, cameraPos, cameraFront, cameraUp, xDiff, yDiff, yaw, pitch, firstMouse);
			movementSpeed = cameraVals.movementSpeed;
			cameraFront = cameraVals.cameraFront;
			cameraUp = cameraVals.cameraUp;
			cameraPos = cameraVals.cameraPos;
			yaw = cameraVals.yaw;
			pitch = cameraVals.pitch;
			firstMouse = cameraVals.firstMouse;
		}
		else {
			auto cameraVals = get_camera_values(state, movementSpeed2, dt, cameraPos2, cameraFront2, cameraUp2, xDiff, yDiff, yaw2, pitch2, firstMouse2);
			movementSpeed2 = cameraVals.movementSpeed;
			cameraFront2 = cameraVals.cameraFront;
			cameraUp2 = cameraVals.cameraUp;
			cameraPos2 = cameraVals.cameraPos;
			yaw2 = cameraVals.yaw;
			pitch2 = cameraVals.pitch;
			firstMouse2 = cameraVals.firstMouse;
		}

		//the matrix that governs where we are looking. replaces the world2camera matrix from exercises
		Mat44f LookAt = lookAt(cameraPos, cameraPos + cameraFront);

		float viewportProjectionChange = state.splitscreen ? 2.f : 1.f;
		Mat44f projection = make_perspective_projection(60.f * 3.1415926f / 180.f,
			(fbwidth/viewportProjectionChange) / float(fbheight),
			0.1f, 100.0f
		);

		if (state.cameraMode == State_::cameraTracking::cameraTrackGround) {
			Vec3f groundCameraPos = { -10.f, 0.1f, -20.f };
			LookAt = lookAt(groundCameraPos, state.vecSpaceshipTranslation);
		}
		else if (state.cameraMode == State_::cameraTracking::cameraTrackFollow) {
			Vec3f followCameraPos = state.vecSpaceshipTranslation;
			followCameraPos.y += 3.f;
			followCameraPos.z += 30.f;
			LookAt = lookAt(followCameraPos, state.vecSpaceshipTranslation);
		}

		Mat44f projCameraWorld = projection * LookAt;


		// Draw scene
		OGL_CHECKPOINT_DEBUG();
		//---------------------------------------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // combingig both tells open gl to clear the color and depth buffer in one go

		//used by all drawn obejcts
		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(kIdentity44f)));

		//query for viewport 1
		glQueryCounter(startView1Query, GL_TIMESTAMP);
		//SETUP FOR THE LANDMASS--------------------------------------------------------------------

		//query for basic rendering
		glQueryCounter(startBasicQuery, GL_TIMESTAMP);

		Vec3f lightDir = normalize(Vec3f{ 0.f, 1.f, -1.f });
		draw_land_mass(prog.programId(), projCameraWorld, normalMatrix, lightDir, tex, vao, static_cast<int>(mesh_vertex_count));

		glQueryCounter(endBasicQuery, GL_TIMESTAMP);

		// Retrieve results for the frame timing
		GLuint64 startBasicTime, endBasicTime;
		glGetQueryObjectui64v(startBasicQuery, GL_QUERY_RESULT, &startBasicTime);
		glGetQueryObjectui64v(endBasicQuery, GL_QUERY_RESULT, &endBasicTime);

		// Calculate the frame rendering time
		GLuint64 basicDuration = endBasicTime - startBasicTime;

		// Output the duration or store it for later use
		basicRendering.duration = basicDuration;
		basicRendering.resultReady = true;

		//SETUP FOR THE LANDING PAD-----------------------------------------------------------------

		//query for instancing.
		glQueryCounter(startInstancingQuery, GL_TIMESTAMP);

		draw_landing_pad(colorShader.programId(), projection, LookAt, landingPadTranslation, numLandingPads, normalMatrix, lightDir, landingpad_vao, static_cast<int>(landingpad_vertex_count),
			state.lightPositions, state.lightColors, state.vertPositions);

		// End timing after instanced rendering
		glQueryCounter(endInstancingQuery, GL_TIMESTAMP);

		// Retrieve results for the frame timing
		GLuint64 startInstancingTime, endInstancingTime;
		glGetQueryObjectui64v(startInstancingQuery, GL_QUERY_RESULT, &startInstancingTime);
		glGetQueryObjectui64v(endInstancingQuery, GL_QUERY_RESULT, &endInstancingTime);

		GLuint64 durationInstancing = endInstancingTime - startInstancingTime;

		//output for Instancing
		instancing.duration = durationInstancing;
		instancing.resultReady = true;

		// end of view port 1
		GLuint64 startView1Time, endView1Time;

		glGetQueryObjectui64v(startView1Query, GL_QUERY_RESULT, &startView1Time);
		glGetQueryObjectui64v(endView1Query, GL_QUERY_RESULT, &endView1Time);

		//output for viewports
		view1Time = startView1Time - endView1Time;
		view1Time = std::abs((view1Time / 10000));
		GLuint64 durationView1;
		durationView1 = startView1Time - endView1Time;
		view1.resultReady = true;
		
		//SETUP FOR THE SPACESHIP-----------------------------------------------------------------
		Mat44f spaceship_translation = landingPadTranslation[1];
		
		// Define the duration of one loop of the animation in seconds.
		const float loop_duration = 10.0f; // Adjust this value as needed.

		if (state.camControl.animationActive) {
			runParticles(listofParticles , 0);
		
			
			// Increment the animation time.
			animation_time += dt;
			// between the range 0 and 1.
			float normalized_time = fmod(animation_time, loop_duration) / loop_duration;
			// acceleration is controlled by the eased time.
			float eased_time = normalized_time * normalized_time * normalized_time * normalized_time;
			// as time goes on speed increases.
			float scaled_eased_time = eased_time * loop_duration;
			// Use eased time for calculations.
			float x = scaled_eased_time;
			float y = (atan(scaled_eased_time * 500) / 15);

			// Calculate the translation for the spaceship's flight movement.
			Mat44f spaceshipFlightMovement = make_translation(Vec3f{ x, y, 0.f });
			state.vecSpaceshipTranslation.x += x;
			state.vecSpaceshipTranslation.y += y;
			spaceship_flight = spaceship_flight * spaceshipFlightMovement;
			spaceship_translation = spaceship_translation * spaceship_flight;

			// Calculate the rotation angle for the spaceship.
			float angle = atan2(y, x) + (271.7f * kPi_ / 180.f);
			
			Mat44f spaceshipRotation = make_rotation_z(angle);
			spaceship_translation = spaceship_translation * spaceshipRotation;
			
			Mat44f lightPosition1 = make_translation(state.lightPositions[0]);

			Mat44f lightPosition2 = make_translation(state.lightPositions[1]);
			Mat44f lightPosition3 = make_translation(state.lightPositions[2]);
			
			lightPosition1 = spaceshipFlightMovement * lightPosition1;
			lightPosition2 = spaceshipFlightMovement * lightPosition2;
			lightPosition3 = spaceshipFlightMovement * lightPosition3;

			state.lightPositions[0] = {lightPosition1(0,3), lightPosition1(1,3), lightPosition1(2,3)};
			state.lightPositions[1] = { lightPosition2(0,3), lightPosition2(1,3), lightPosition2(2,3) };
			state.lightPositions[2] = { lightPosition3(0,3), lightPosition3(1,3), lightPosition3(2,3) };
		
			draw_particles(particleShader.programId(), projection, LookAt, spaceship_translation * make_translation({0.0f,-0.1f,0.f}), normalMatrix,
				listofParticles, particleVAOList, particle_vertexCount, particleMovement);
			

		}
		else if (!state.camControl.animationActive) {
			animation_time = 0.f;
			spaceship_flight = make_translation(Vec3f{ 0.f, 0.f, 0.f });
		}

		// DRAW SPACESHIP TO SHADERS
		// query for custom model
		glQueryCounter(startCustomModelQuery, GL_TIMESTAMP);
		draw_spaceship(colorShader.programId(), projection, LookAt, spaceship_translation, normalMatrix,lightDir, spaceship_vao, static_cast<int>(spaceship_vertex_count),
			state.lightPositions, state.lightColors, state.vertPositions);

		// End timing after custom model rendering
		glQueryCounter(endCustomModelQuery, GL_TIMESTAMP);

		// Retrieve results for the frame timing
		GLuint64 startCustomModelTime, endCustomModelTime;
		glGetQueryObjectui64v(startCustomModelQuery, GL_QUERY_RESULT, &startCustomModelTime);
		glGetQueryObjectui64v(endCustomModelQuery, GL_QUERY_RESULT, &endCustomModelTime);

		GLuint64 durationCustomModel = endCustomModelTime - startCustomModelTime;
		//output for custom model
		customModel.duration = durationCustomModel;
		customModel.resultReady = true;

		//reset state
		glBindVertexArray(0);
		glUseProgram(0);

		OGL_CHECKPOINT_DEBUG();

		//VIEWPORT 2-------------------------------------------------------------------------------------------------------------------
		if (state.splitscreen) {
			//query for viewport 2
			glQueryCounter(startView2Query, GL_TIMESTAMP);

			LookAt = lookAt(cameraPos2, cameraPos2 + cameraFront2);

			if (state.cameraMode2 == State_::cameraTracking::cameraTrackGround) {
				Vec3f groundCameraPos = { -10.f, 0.1f, -20.f };
				LookAt = lookAt(groundCameraPos, state.vecSpaceshipTranslation);
			}
			else if (state.cameraMode2 == State_::cameraTracking::cameraTrackFollow) {
				Vec3f followCameraPos = state.vecSpaceshipTranslation;
				followCameraPos.y += 3.f;
				followCameraPos.z += 30.f;
				LookAt = lookAt(followCameraPos, state.vecSpaceshipTranslation);
			}

			projCameraWorld = projection * LookAt;

			glViewport( static_cast<GLsizei>(fbwidth/2), 0, static_cast<GLsizei>(fbwidth/ 2.f), static_cast<GLsizei>(fbheight));

			//SETUP FOR THE LANDMASS--------------------------------------------------------------------
			draw_land_mass(prog.programId(), projCameraWorld, normalMatrix, lightDir, tex, vao, static_cast<int>(mesh_vertex_count));
			// DRAW OTHER ITEMS----------------------------------------------------------------------------

			draw_landing_pad(colorShader.programId(), projection, LookAt, landingPadTranslation, numLandingPads, normalMatrix, lightDir, landingpad_vao, static_cast<int>(landingpad_vertex_count),
				state.lightPositions, state.lightColors, state.vertPositions);

			draw_spaceship(colorShader.programId(), projection, LookAt, spaceship_translation, normalMatrix, lightDir, spaceship_vao, static_cast<int>(spaceship_vertex_count),
				state.lightPositions, state.lightColors, state.vertPositions);

			// end the time for view port 2
			glQueryCounter(endView2Query, GL_TIMESTAMP);

			// Retrieve results for the frame timing
			GLuint64  startView2Time, endView2Time;
			glGetQueryObjectui64v(startView2Query, GL_QUERY_RESULT, &startView2Time);
			glGetQueryObjectui64v(endView2Query, GL_QUERY_RESULT, &endView2Time);
			GLuint64 durationView2 = (endView2Time - startView2Time);

			//output for viewports
			view2.duration = durationView2;
			view2.resultReady = true;
		}

		// End frame query
		glQueryCounter(endFrameQuery, GL_TIMESTAMP);
		GLuint64 startFrameTime, endFrameTime;
		glGetQueryObjectui64v(startFrameQuery, GL_QUERY_RESULT, &startFrameTime);
		glGetQueryObjectui64v(endFrameQuery, GL_QUERY_RESULT, &endFrameTime);

		// Calculate the frame rendering time
		GLuint64 frameDuration = endFrameTime - startFrameTime;

		// Output the duration or store it for later 
		FullRender.duration = frameDuration;
		FullRender.resultReady = true;

		//end frame to frame timer
		auto end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - last);

		glViewport(0, 0, static_cast<GLsizei>(fbwidth), static_cast<GLsizei>(fbheight));
		glUseProgram(uiShader.programId());
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (inResetBtnArea) {
			resetButtonVao = create_rectangle_vao(buttonReset, Vec4f{ 0.f, 0.3f, 1.f, 0.5f});
		}
		else {
			resetButtonVao = create_rectangle_vao(buttonReset, Vec4f{ 0.f, 0.f, 1.f, 0.5f});
		}

		if (inLaunchBtnArea) {
			launchButtonVao = create_rectangle_vao(buttonLaunch, Vec4f{1.f, 0.3f, 0.f, 0.5f});
		}
		else {
			launchButtonVao = create_rectangle_vao(buttonLaunch, Vec4f{1.f, 0.f, 0.f, 0.5f});
		}
		
		if (state.mouseLeftPressed) {
			bool clickedLaunchButton = is_mouse_in_area(state.mousePressedX, state.mousePressedY, launchButtonBoundingBox);
			if (clickedLaunchButton) {
				launchButtonVao = create_rectangle_vao(buttonLaunch, Vec4f{1.f, 1.f, 0.f, 0.5f});
				state.camControl.animationActive = true;
			}
			bool clickedResetButton = is_mouse_in_area(state.mousePressedX, state.mousePressedY, resetButtonBoundingBox);
			if (clickedResetButton) {
				resetButtonVao = create_rectangle_vao(buttonReset, Vec4f{0.f, 1.f, 1.f, 0.5f});
				state.camControl.animationActive = false;
				state.vecSpaceshipTranslation = secondLandingpadTranslation;
				state.lightPositions = get_lightpositions();
			}
		}


		glBindVertexArray(launchButtonVao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(resetButtonVao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisable(GL_BLEND);

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(launchButtonOutlineVao);
		glDrawArrays(GL_LINES, 0, 8);
		glBindVertexArray(resetButtonOutlineVao);
		glDrawArrays(GL_LINES, 0, 8);
		glEnable(GL_DEPTH_TEST);

		// Display results
		glfwSwapBuffers( window );
	}

	// Output performance results
	std::cout << "GPU Performance Table:\n";
	std::cout << "Section\t\t\tDuration (ns)\n";
	std::cout << "-------------------------------------\n";
	std::cout << "Full Render" << "\t\t" << FullRender.duration << "\n";
	std::cout << "Basic Rendering: " << "\t" << basicRendering.duration << "\n";
	std::cout << "Instancing: " << "\t\t" << instancing.duration << "\n";
	std::cout << "Custom Model" << "\t\t" << customModel.duration << "\n";
	std::cout << "One View" << "\t\t" << view1Time << "\n";
	std::cout << "Both Views: " << "\t\t" << view2.duration << "\n\n";

	//output frame to frame results
	std::cout << "Frame to Frame Performance Table:\n";
	std::cout << "Section\t\t\tDuration (ns)\n";
	std::cout << "-------------------------------------\n";
	std::cout << "Multiple Frames" << "\t\t" << duration.count() << "\n";



	// Cleanup.
	//TODO: additional cleanup
	
	return 0;
}
catch( std::exception const& eErr )
{
	std::fprintf( stderr, "Top-level Exception (%s):\n", typeid(eErr).name() );
	std::fprintf( stderr, "%s\n", eErr.what() );
	std::fprintf( stderr, "Bye.\n" );
	return 1;
}


namespace
{
	void glfw_callback_error_( int aErrNum, char const* aErrDesc )
	{
		std::fprintf( stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum );
	}

	void glfw_callback_key_( GLFWwindow* aWindow, int aKey, int, int aAction, int mode)
	{
		if( GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction )
		{
			glfwSetWindowShouldClose( aWindow, GLFW_TRUE );
			return;
		}

		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			// R-key reloads shaders.
			if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
			{
				if (state->prog)
				{
					try
					{
						state->prog->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}
				}
			}

			//F starts animation
			if (GLFW_KEY_F == aKey && GLFW_PRESS == aAction) {
				state->camControl.animationActive = true;
			}
			//R resets animation, bringing spaceship to starting point and stopping it
			else if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction) {
				state->camControl.animationActive = false;
				state->lightPositions = get_lightpositions();
				//same as second landing pad location, should try not to hardcode
				state->vecSpaceshipTranslation = { 15.f, -0.95f, -10.f };

			}
			//V splitscreens the view
			else if (GLFW_KEY_V == aKey && GLFW_PRESS == aAction) {
				state->splitscreen = !state->splitscreen;
			}
			//C changes camera
			else if (GLFW_KEY_C == aKey && GLFW_PRESS == aAction) {
				if (mode == GLFW_MOD_SHIFT) {
					//only let switch if the split screen is enabled
					if (state->splitscreen) {
						state->switchscreen = !state->switchscreen;
					}
				}
				else {
					//depending on witch screen we're on then we change that screens camera track
					if (!state->switchscreen) {
						switch (state->cameraMode) {
						case State_::cameraTracking::cameraNormal:
							state->cameraMode = State_::cameraTracking::cameraTrackFollow;
							break;

						case State_::cameraTracking::cameraTrackFollow:
							state->cameraMode = State_::cameraTracking::cameraTrackGround;
							break;

						case State_::cameraTracking::cameraTrackGround:
							state->cameraMode = State_::cameraTracking::cameraNormal;
							break;
						}
					}
					else {
						switch (state->cameraMode2) {
						case State_::cameraTracking::cameraNormal:
							state->cameraMode2 = State_::cameraTracking::cameraTrackFollow;
							break;

						case State_::cameraTracking::cameraTrackFollow:
							state->cameraMode2 = State_::cameraTracking::cameraTrackGround;
							break;

						case State_::cameraTracking::cameraTrackGround:
							state->cameraMode2 = State_::cameraTracking::cameraNormal;
							break;
						}
					}
				}
			}

			// Space toggles camera
			if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.cameraActive = !state->camControl.cameraActive;

				if (state->camControl.cameraActive)
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				else {
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					state->camControl.moveForward = false;
					state->camControl.moveBackward = false;
					state->camControl.moveLeft = false;
					state->camControl.moveRight = false;
				}
			}

			// Camera controls if camera is active
			if (state->camControl.cameraActive)
			{
				if (GLFW_KEY_W == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveForward = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveForward = false;
				}
				else if (GLFW_KEY_S == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveBackward = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveBackward = false;
				}
				else if (GLFW_KEY_A == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveLeft = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveLeft = false;
				}
				else if (GLFW_KEY_D == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveRight = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveRight = false;
				}
				else if (GLFW_KEY_E == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveUp = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveUp = false;
				}
				else if (GLFW_KEY_Q == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveDown = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveDown = false;
				}
				else if (GLFW_KEY_LEFT_SHIFT == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.speedUp = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.speedUp = false;
				}
				else if (GLFW_KEY_LEFT_CONTROL == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.speedDown = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.speedDown = false;
				}
			}
		}

	}

	void glfw_callback_mouseclick_(GLFWwindow* aWindow, int button, int action, int ) {
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow))) {
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				state->mouseLeftPressed = true;
				double xpos{}, ypos{};
				glfwGetCursorPos(aWindow, &xpos, &ypos);
				state->mousePressedX = static_cast<float>(xpos);
				state->mousePressedY = static_cast<float>(ypos);

			}
			
			else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
				state->mouseLeftPressed = false;
			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camControl.cameraActive)
			{
				auto const dx = float(aX - state->camControl.lastX);
				auto const dy = float(aY - state->camControl.lastY);

				state->camControl.phi += dx * kMouseSensitivity_;

				state->camControl.theta += dy * kMouseSensitivity_;
				if (state->camControl.theta > kPi_ / 2.f)
					state->camControl.theta = kPi_ / 2.f;
				else if (state->camControl.theta < -kPi_ / 2.f)
					state->camControl.theta = -kPi_ / 2.f;
			}

			state->camControl.lastX = state->camControl.currentX;
			state->camControl.lastY = state->camControl.currentY;

			state->camControl.currentX = float(aX);
			state->camControl.currentY = float(aY);
		}
	}

	float radians(float degrees) {
		return degrees * kPi_ / 180;
	}

	Mat44f lookAt(Vec3f eye, Vec3f target) {
		//eye - camera position
		//target - where the camera is looking
		//up - which vector points directly up

		Vec3f cameraDirection = normalize(eye - target);
		Vec3f flat_up = { 0.f, 1.0f, 0.f };
		Vec3f cameraRight = normalize(cross(flat_up, cameraDirection));
		Vec3f cameraUp = normalize(cross(cameraDirection, cameraRight));

		Mat44f rotationMatrix{
			{ cameraRight.x, cameraRight.y, cameraRight.z, 0.f, //cameraRight vector ( correct for no mouse movement)
				cameraUp.x, cameraUp.y, cameraUp.z, 0.f, //cameraUp vector (pretty sure this is definitely correct)
				cameraDirection.x, cameraDirection.y, cameraDirection.z, 0.f, //direction vector 
				0.f, 0.f, 0.f, 1.f
			} };

		Mat44f cameraPositionMatrix = {
			{
				1.f, 0.f, 0.f, -eye.x,
				0.f, 1.f, 0.f, -eye.y,
				0.f, 0.f, 1.f, -eye.z,
				0.f, 0.f, 0.f, 1.f,
			}
		};

		return rotationMatrix * cameraPositionMatrix;
	}

	void draw_land_mass(GLuint shaderId, Mat44f projCameraWorld, Mat33f normalMatrix, Vec3f lightDir, GLuint tex, GLuint vao, int vertexCount) {
		glUseProgram(shaderId);

		//vertex shader parameters
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);

		//fragment shader parameters
		glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 0.9f, 0.9f, 0.6f);
		glUniform3f(4, 0.05f, 0.05f, 0.05f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount); 
	}

	void draw_particles(GLuint shaderId, Mat44f projection, Mat44f lookAt, Mat44f translation, Mat33f normalMatrix,
		std::vector<Particle> listofParticles, std::vector<GLuint> vao, int vertexCount, std::vector<Vec3f> particleMovement)
	{
		
		//Vec3f movement = particleMovement * translation.v;
		//additive blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);


		glUseProgram(shaderId);

		

		for (unsigned int i = 0; i < listofParticles.size(); ++i)
		{
			if (listofParticles[i].lifespan > 0.0f)
			{
				
				glUniformMatrix4fv(0, 1, GL_TRUE, projection.v);
				glUniformMatrix4fv(1, 1, GL_TRUE, lookAt.v);
				glUniformMatrix4fv(5, 1, GL_TRUE, translation.v); //* particleMovement[i]);
				glUniformMatrix3fv(6, 1, GL_TRUE, normalMatrix.v);

				//fragment shader
				glBindVertexArray(vao[i]);
				glDrawArrays(GL_TRIANGLES, 0, vertexCount);

			}


		}
		//revert back to normal
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	}

	void draw_landing_pad(GLuint shaderId, Mat44f projection, Mat44f lookAt, Mat44f translations[], int numTranslations, 
		Mat33f normalMatrix, Vec3f lightDir, GLuint vao, int vertexCount,
		std::vector<Vec3f> lightPositions, std::vector<Vec3f> lightColors, std::vector<Vec3f> vertPositions) {

		// Convert the vector to a plain array
		GLfloat* lightPositionsArray = new GLfloat[lightPositions.size() * 3];
		for (size_t i = 0; i < lightPositions.size(); ++i) {
			lightPositionsArray[i * 3] = lightPositions[i].x;
			lightPositionsArray[i * 3 + 1] = lightPositions[i].y;
			lightPositionsArray[i * 3 + 2] = lightPositions[i].z;
		}

		GLfloat* lightColorsArray = new GLfloat[lightColors.size() * 3];
		for (size_t i = 0; i < lightColors.size(); ++i) {
			lightColorsArray[i * 3] = lightColors[i].x;
			lightColorsArray[i * 3 + 1] = lightColors[i].y;
			lightColorsArray[i * 3 + 2] = lightColors[i].z;
		}

		GLfloat* vertPositionsArray = new GLfloat[vertPositions.size() * 3];
		for (size_t i = 0; i < vertPositions.size(); ++i) {
			vertPositionsArray[i * 3] = vertPositions[i].x;
			vertPositionsArray[i * 3 + 1] = vertPositions[i].y;
			vertPositionsArray[i * 3 + 2] = vertPositions[i].z;
		}

		glUseProgram(shaderId);

		//vertex shader parameters
		glUniformMatrix4fv(0, 1, GL_TRUE, projection.v);
		glUniformMatrix4fv(1, 1, GL_TRUE, lookAt.v);
		glUniformMatrix3fv(6, 1, GL_TRUE, normalMatrix.v);

		//fragment shader parameters
		glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 0.9f, 0.9f, 0.6f);
		glUniform3f(4, 0.05f, 0.05f, 0.05f);
		glUniform3f(7, 3.0f, 3.0f, 3.0f);
		glUniform3fv(8, static_cast<GLsizei>(lightPositions.size()), lightPositionsArray);
		glUniform3fv(11, static_cast<GLsizei>(lightColors.size()), lightColorsArray);
		glUniform3fv(15, static_cast<GLsizei>(vertPositions.size()), vertPositionsArray);

		glBindVertexArray(vao);
		for (int i = 0; i < numTranslations; i++) {
			//change translation
			glUniformMatrix4fv(5, 1, GL_TRUE, translations[i].v);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount); 
		} 

		delete[] lightColorsArray;
		delete[] lightPositionsArray;
		delete[] vertPositionsArray;
	}

	GLuint create_rectangle_vao(std::vector<Vec2f> kPositions, Vec4f color) {

		GLuint positionVBO;
		glGenBuffers(1, &positionVBO); //generate the buffer pointer and give its value to positionVBO
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO); //we make sure its of type GL_ARRAY_BUFFER which all VBOs are
		glBufferData(GL_ARRAY_BUFFER, kPositions.size() * sizeof(Vec2f), kPositions.data(), GL_STATIC_DRAW); // we then add data to it


		std::vector<Vec4f> kColors(kPositions.size(), color);

		GLuint colorVBO;
		glGenBuffers(1, &colorVBO); //generate the buffer pointer and give its value to positionVBO
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO); //we make sure its of type GL_ARRAY_BUFFER which all VBOs are
		glBufferData(GL_ARRAY_BUFFER, kColors.size() * sizeof(Vec4f), kColors.data(), GL_STATIC_DRAW); // we then add data to it

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,0, 0 );
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0 );
		glEnableVertexAttribArray(1);

		return vao;
	}

	void draw_spaceship(GLuint shaderId, Mat44f projection, Mat44f lookAt, Mat44f translation, Mat33f normalMatrix, Vec3f lightDir, GLuint vao, int vertexCount,
		std::vector<Vec3f> lightPositions, std::vector<Vec3f> lightColors, std::vector<Vec3f> vertPositions) {
		glUseProgram(shaderId);

		GLfloat* lightPositionsArray = new GLfloat[lightPositions.size() * 3];
		for (size_t i = 0; i < lightPositions.size(); ++i) {
			lightPositionsArray[i * 3] = lightPositions[i].x;
			lightPositionsArray[i * 3 + 1] = lightPositions[i].y;
			lightPositionsArray[i * 3 + 2] = lightPositions[i].z;
		}

		GLfloat* lightColorsArray = new GLfloat[lightColors.size() * 3];
		for (size_t i = 0; i < lightColors.size(); ++i) {
			lightColorsArray[i * 3] = lightColors[i].x;
			lightColorsArray[i * 3 + 1] = lightColors[i].y;
			lightColorsArray[i * 3 + 2] = lightColors[i].z;
		}

		GLfloat* vertPositionsArray = new GLfloat[vertPositions.size() * 3];
		for (size_t i = 0; i < vertPositions.size(); ++i) {
			vertPositionsArray[i * 3] = vertPositions[i].x;
			vertPositionsArray[i * 3 + 1] = vertPositions[i].y;
			vertPositionsArray[i * 3 + 2] = vertPositions[i].z;
		}

		//vertex shader parameters
		glUniformMatrix4fv(0, 1, GL_TRUE, projection.v);
		glUniformMatrix4fv(1, 1, GL_TRUE, lookAt.v);
		glUniformMatrix4fv(5, 1, GL_TRUE, translation.v);
		glUniformMatrix3fv(6, 1, GL_TRUE, normalMatrix.v);

		//fragment shader parameters
		glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 0.9f, 0.9f, 0.6f);
		glUniform3f(4, 0.05f, 0.05f, 0.05f);
		glUniform3f(7, 3.0f, 3.0f, 3.0f);
		glUniform3fv(8, static_cast<GLsizei>(lightPositions.size()), lightPositionsArray);
		glUniform3fv(11, static_cast<GLsizei>(lightColors.size()), lightColorsArray);
		glUniform3fv(15, static_cast<GLsizei>(vertPositions.size()), vertPositionsArray);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		delete[] lightColorsArray;
		delete[] lightPositionsArray;
		delete[] vertPositionsArray;
	}
	
	
	
	CameraValues get_camera_values(State_ &state, float movementSpeed, float dt, Vec3f cameraPos, Vec3f cameraFront, Vec3f cameraUp,
		float xDiff, float yDiff, float yaw, float pitch, bool firstMouse) {

		if (state.camControl.cameraActive) {

			if (firstMouse) {
				state.camControl.lastX = state.camControl.currentX;
				state.camControl.lastY = state.camControl.currentY;
				firstMouse = false;
			}

			float sensitivity = 1.f;

			xDiff *= sensitivity;
			yDiff *= sensitivity;

			yaw += xDiff;
			pitch += yDiff;

			//make sure we dont need to flip the LookAt matrix if we go too far up or down
			if (pitch > 89.f) pitch = 89.f;
			if (pitch < -89.f) pitch = -89.f;

			Vec3f direction{};
			//for mouse movement (change where the camera is looking
			direction.x = cos(radians(yaw)) * cos(radians(pitch));
			direction.y = sin(radians(pitch));
			direction.z = sin(radians(yaw)) * cos(radians(pitch));

			cameraFront = normalize(direction);
		}

		Vec3f constantUp = { 0.f, 1.f, 0.f };

		if (state.camControl.speedUp) {
			movementSpeed += movementSpeed * dt;
		}
		if (state.camControl.speedDown) {
			movementSpeed -= movementSpeed * dt;
		}

		if (state.camControl.moveForward) {
			cameraPos += (movementSpeed * cameraFront) * dt;
		}
		else if (state.camControl.moveBackward) {
			cameraPos -= (movementSpeed * cameraFront) * dt;
		}
		else if (state.camControl.moveLeft) {
			cameraPos -= normalize(cross(cameraFront, cameraUp)) * movementSpeed * dt;
		}
		else if (state.camControl.moveRight) {
			cameraPos += normalize(cross(cameraFront, cameraUp)) * movementSpeed * dt;
		}
		else if (state.camControl.moveUp) {
			cameraPos += (movementSpeed * constantUp) * dt;
		}
		else if (state.camControl.moveDown) {
			cameraPos -= (movementSpeed * constantUp) * dt;
		}

		return CameraValues{ cameraFront, cameraUp, cameraPos, movementSpeed, yaw, pitch, firstMouse };
	}


	std::vector<Vec3f> get_lightpositions() {
		std::vector<Vec3f> lightPositions;
		lightPositions.push_back({ 17.75f, 2.5f, -9.f }); //right side of  the rocket
		lightPositions.push_back({ 15.f, 5.95f, -10.f }); //position for top of rocket
		lightPositions.push_back({ 11.75f, 2.5f, -10.f }); //left side of the rocket
		
		return lightPositions;
	}

	std::vector<Vec3f> get_lightcolors() {
		std::vector<Vec3f> lightColors;
		lightColors.push_back({ 0.8f, 0.0f, 0.0f });  // red
		lightColors.push_back({ 0.0f, 0.8f, 0.0f });  // green
		lightColors.push_back({ 0.0f, 0.0f, 0.8f });  // blue
		return lightColors;
	}

	std::vector<Vec3f> get_vertpositions() {
		std::vector<Vec3f> vertPositions;
		vertPositions.push_back({ 16.90f, 2.5f, -10.f }); // right side of rocket
		vertPositions.push_back({ 15.f, 4.5f, -10.f }); //top of rocket vertex
		vertPositions.push_back({ 14.50f, 2.5f, -10.f });//left side of the rocket
		return vertPositions;
	}

	Vec2f translate_2d_to_xy(float x_2d, float y_2d, float screenWidth, float screenHeight) {
		float temp_x2d = x_2d + 1.f;
		float temp_y2d = y_2d + 1.f;

		float translated_x = (temp_x2d * 10.f) * (float(screenWidth) / 20.f);
		float y = temp_y2d * 10.f;
		float r = float(screenHeight) / 20.f;
		float translated_y = float(screenHeight) - (y * r);

		return Vec2f{ translated_x, translated_y };
	}

	bool is_mouse_in_area(float mouse_x, float mouse_y, std::vector<Vec2f> boundingCoords) {
		Vec2f tl = boundingCoords[0];
		Vec2f tr = boundingCoords[1];
		Vec2f bl = boundingCoords[2];
		Vec2f br = boundingCoords[3];

		return mouse_x >= tl.x && mouse_x <= tr.x && mouse_y >= tl.y && mouse_y <= bl.y;
	}

	std::vector<Vec2f> generate_outlines(std::vector<Vec2f> rectangle) {
		//kPositions.push_back(Vec2f{ -0.9f, 0.9f }); //tl
		//kPositions.push_back(Vec2f{ -0.9f, 0.8f }); //bl
		//kPositions.push_back(Vec2f{ -0.7f, 0.8f }); //br

		//kPositions.push_back(Vec2f{ -0.9f, 0.9f });
		//kPositions.push_back(Vec2f{ -0.7f, 0.9f }); //tr
		//kPositions.push_back(Vec2f{ -0.7f, 0.8f });
		
		std::vector<Vec2f> outlines{};
		Vec2f tl = rectangle[0];
		Vec2f tr = rectangle[4];
		outlines.push_back(tl);
		outlines.push_back(tr);

		//tr to br
		Vec2f br = rectangle[2];
		outlines.push_back(tr);
		outlines.push_back(br);


		//br to bl
		outlines.push_back(rectangle[2]);
		outlines.push_back(rectangle[1]);

		////bl to tl
		outlines.push_back(rectangle[0]);
		outlines.push_back(rectangle[1]);

		return outlines;
	}

}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if( window )
			glfwDestroyWindow( window );
	}
}

