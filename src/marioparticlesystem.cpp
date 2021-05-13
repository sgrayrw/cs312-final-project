#include "util/AGL.h"
#include "util/AGLM.h"
#include "marioparticlesystem.h"

using namespace agl;

void MarioParticleSystem::createParticles(int size) {
    mTexture = theRenderer.loadTexture("../textures/mario.png");

    Particle mario {
            spawn,
            glm::vec3(0),
            glm::vec3(0, -9.8, 0), // gravity
            glm::vec4(1),
            0.2,
            glm::vec3(0.2, 1, 1)
    };
    mParticles.push_back(mario);

    Particle goomba {
            glm::vec3(0.5, 0, 0),
            glm::vec3(0.1, 0, 0),
            glm::vec3(0),
            glm::vec4(1),
            0.2,
            glm::vec3(1),
    };
    mParticles.push_back(goomba);
}

void MarioParticleSystem::update(float dt) {
    updateMario(dt);
    updateGoomba(dt);
    handleCollision();
}

void MarioParticleSystem::updateMario(float dt) {
    Particle& mario = mParticles[0];

    // jump
    if (pressedKeys.find(GLFW_KEY_UP) != pressedKeys.end() && pressedKeys.at(GLFW_KEY_UP)) {
        mario.vel.y = 2;
    }

    // left and right
    if (pressedKeys.find(GLFW_KEY_LEFT) != pressedKeys.end() && pressedKeys.at(GLFW_KEY_LEFT)) {
        mario.force.x = -1;
    } else if (pressedKeys.find(GLFW_KEY_RIGHT) != pressedKeys.end() && pressedKeys.at(GLFW_KEY_RIGHT)) {
        mario.force.x = 1;
    } else {
        mario.force.x = 0;
    }

    // friction
    if (mario.vel.x != 0) {
        mario.force.x += -glm::sign(mario.vel.x) * 0.3;
    }

    // update pos and vel
    mario.pos = mario.pos + dt * mario.vel;
    mario.vel = mario.vel + dt * mario.force / mario.mass;

    // base
    if (mario.pos.y <= 0) {
        mario.pos.y = 0;
        mario.vel.y = glm::max(0.0f, mario.vel.y);
    }
}

void MarioParticleSystem::updateGoomba(float dt) {
    Particle& goomba = mParticles[1];
    goomba.pos = goomba.pos + dt * goomba.vel;

    if (glm::abs(goomba.pos.x - goomba.initPos.x) > 0.15) {
        goomba.vel.x *= -1;
    }
}

void MarioParticleSystem::setKey(int key, int action) {
    pressedKeys[key] = (action != GLFW_RELEASE);
}

// adapted from https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection
Collision MarioParticleSystem::collide(const Particle& from, const Particle& to) {
    bool collisionX = from.pos.x + from.size >= to.pos.x && to.pos.x + to.size >= from.pos.x;
    bool collisionY = from.pos.y + from.size >= to.pos.y && to.pos.y + to.size >= from.pos.y;
    if (collisionX && collisionY) {
        return (from.pos.y > to.pos.y) ? TOP_COLLISION : BOTTOM_COLLISION;
    } else {
        return NO_COLLISION;
    }
}

void MarioParticleSystem::handleCollision() {
    Particle& mario = mParticles[0];
    Particle& goomba = mParticles[1];
    Collision collision = collide(mario, goomba);
    if (collision == TOP_COLLISION) {
        mario.vel = glm::reflect(mario.vel, glm::vec3(0, 1, 0));
    } else if (collision == BOTTOM_COLLISION) {
        mario.vel = mario.pos = spawn;
    }
}