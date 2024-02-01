#include "particle.hpp"

#include<cmath>
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"

float random(float upper, float lower)
{

    float range = (upper - lower);
    float randomDouble = lower + (range * ((float)rand()) / (RAND_MAX));

    return randomDouble;
}

Particle createParticles( Mat44f aPreTransform)
{
    //coordinate of other landing pad
    
    
     // Vec3f coords = { -20.f + randomise, -0.95f + randomise, -30.f + randomise };

    
    Particle particles;

    Vec3f colors = { 0.3f,0.3f,0.3f };
    std::vector<Vec3f> pos;
    std::vector<Vec3f> normals;

    //create the positions for the smoke quad
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
        

    // populate the positions of the particles
    std::vector<Vec3f> col(pos.size(), colors);
    particles.particledraw.positions = std::move(pos);
    particles.particledraw.normals = std::move(normals);
    particles.particledraw.colors = std::move(col);


    float getAcc = static_cast<float>(0.1 * random(-5, 5));



    Vec3f accelerationAdd = { getAcc , getAcc, getAcc };
    particles.acceleration = accelerationAdd;


    //last 1 second for each quad
    particles.lifespan = 1.0f;


    
    return particles;

}


void runParticles( std::vector<Particle>& particles, unsigned int lastUsedParticle)
{
    //create particles outside
     
    // check particle life, respawn if dead
    for (unsigned int i = 0; i < particles.size(); ++i)
    {
        unsigned int unusedParticle = FirstUnusedParticle(particles, lastUsedParticle);
        RespawnParticle(particles, unusedParticle, 2.0f);
        lastUsedParticle = unusedParticle;
    }


    //update all particles
    float dt = static_cast<float>(0.1);
    Vec3f colorChange = { dt,dt,dt };
    
    for (unsigned int i = 0; i < particles.size(); ++i)
    {
        
        particles[i].lifespan -= dt;
        if (particles[i].lifespan > 0.0f)
        {
            //reduce speeding of particle
            for (unsigned int j = 0; j < particles[i].particledraw.positions.size(); ++j) {
                particles[i].particledraw.positions[j] -= particles[i].acceleration;
                particles[i].particledraw.normals[j] -= particles[i].acceleration;
                particles[i].particledraw.colors[j] -= colorChange;
            }
        }
    }

}

unsigned int FirstUnusedParticle(const std::vector<Particle>& particles, unsigned int lastUsedParticle)
{
    // Search from last used particle
    for (unsigned int i = lastUsedParticle; i < particles.size(); ++i)
    {
        if (particles[i].lifespan <= 0.0f)
        {
            return i;
        }
    }

    // linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i)
    {
        if (particles[i].lifespan <= 0.0f)
        {
            return i;
        }
    }

    // Override the first particle if all others are alive
    return 0;
}

void RespawnParticle(std::vector<Particle>&particle, unsigned int unusedParticle, float offset)
{
    // Set initial values for the particle
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);

    //coordinate of other landing pad
   // Vec3f coords = { -20.f, -0.95f, -30.f };

    // will have to change to match exhaust range
    Vec3f newPosition;
    newPosition.x = random + offset;
    newPosition.y = random + offset;
    newPosition.z = 0.0f;  // Assuming z remains unchanged

    Vec3f newColor;
    newColor.x = rColor;
    newColor.y = rColor;
    newColor.z = rColor;

    float newLife = 1.0f;

    //offset for all positions using make translation for positions and normals
    //translation 
    Mat44f translation = make_translation(newPosition);
    Mat33f N = mat44_to_mat33(transpose(invert(translation)));
    for (auto& p : particle[unusedParticle].particledraw.positions) {
        Vec4f p4{ p.x, p.y, p.z, 1.f };
        Vec4f t = translation * p4;
        p = { t.x, t.y, t.z };
    }

    for (auto& n : particle[unusedParticle].particledraw.normals) {
        Vec3f p3{ n.x, n.y, n.z };
        Vec3f t = N * p3;

        Vec3f b = Vec3f{ t.x, t.y, t.z };
        n = b;
    }


    particle[unusedParticle].acceleration = particle[unusedParticle].acceleration * 0.1f;
    particle[unusedParticle].lifespan = newLife;
}

