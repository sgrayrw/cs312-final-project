#include "util/AGL.h"
#include "util/AGLM.h"
#include "marioparticlesystem.h"

using namespace agl;

void MarioParticleSystem::createParticles(int size) {
    mTexture = theRenderer.loadTexture("../textures/mario.png");

    Particle mario = {
            glm::vec3(0),
            glm::vec3(0),
            glm::vec3(0, -9.8, 0), // gravity
            glm::vec4(1),
            0.2,
            1
    };
    mParticles.push_back(mario);
}

void MarioParticleSystem::update(float dt) {
    Particle &mario = mParticles[0];

    if (lastKeyAction == GLFW_RELEASE) {
        mario.vel.x = 0;
    } else {
        switch (lastPressedKey) {
            case GLFW_KEY_RIGHT:
                mario.vel.x = 1;
                break;
            case GLFW_KEY_LEFT:
                mario.vel.x = -1;
                break;
            case GLFW_KEY_UP:
                mario.vel.y = 2;
        }
    }

    mario.pos = mario.pos + dt * mario.vel;
    mario.vel = mario.vel + dt * mario.force / mario.mass;

    if (mario.pos.y <= 0) {
        mario.vel.y = glm::max(0.0f, mario.vel.y);
    }
}

void MarioParticleSystem::setKey(int key, int action) {
    lastPressedKey = key;
    lastKeyAction = action;
}
