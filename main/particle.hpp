#ifndef PARTICLE_HPP
#define PARTICLE_HPP


#include <vector>
#include "../vmlib/mat44.hpp"
#include "../vmlib/vec3.hpp"
#include "../vmlib/vec2.hpp"
#include "simple_mesh.hpp"

struct Particle
{
	SimpleMeshData particledraw;
	Vec3f acceleration;
	float lifespan;
	
};

float random(float upper, float lower);
Particle createParticles(Mat44f aPreTransform);
void runParticles(std::vector<Particle>&particles, unsigned int lastUsedParticle);
unsigned int FirstUnusedParticle(const std::vector<Particle>& particles, unsigned int lastUsedParticle);
void RespawnParticle(std::vector<Particle>& particles, unsigned int unusedParticle, float offset);

#endif