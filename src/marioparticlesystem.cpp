#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include "util/AGL.h"
#include "marioparticlesystem.h"

using namespace agl;

void MarioParticleSystem::createParticles(int size) {
    mTexture = theRenderer.loadTexture("../textures/mario.png");

    Particle mario {
            spawn,
            glm::vec3(0, 0.5, 0),
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

    Particle brick {
            glm::vec3(-0.2, 0.3, 0),
            glm::vec3(0),
            glm::vec3(0),
            glm::vec4(1),
            0.2,
            glm::vec3(1),
    };
    mParticles.push_back(brick);

    for (int i = 0; i < 10; ++i) {
        Particle baseWall {
                glm::vec3(-1 + i * 0.2, -0.2, 0),
                glm::vec3(0),
                glm::vec3(0),
                glm::vec4(1),
                0.2,
                glm::vec3(1),
        };
        mParticles.push_back(baseWall);
    }
}

void MarioParticleSystem::update(float dt) {
    updateMario(dt);
    updateGoomba(dt);
    handleCollision();
}

void MarioParticleSystem::updateMario(float dt) {
    Particle& mario = mParticles[0];

    // jump
    if (pressedKeys.find(GLFW_KEY_UP) != pressedKeys.end() && pressedKeys.at(GLFW_KEY_UP) &&
            mario.vel.y >= 0 && mario.pos.y - mario.baseY < 0.4) {
        mario.vel.y = 2;
        mario.onBrick = false;
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

    // clamp
    mario.vel.x = glm::clamp(mario.vel.x, -1.0f, 1.0f);

    // base
    if (mario.onBrick) {
        mario.pos.y = mario.baseY;
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
        glm::vec2 compass[] = {
                glm::vec2(0.0f, 1.0f),	// up
                glm::vec2(0.0f, -1.0f),	// down
                glm::vec2(-1.0f, 0.0f),	// left
                glm::vec2(1.0f, 0.0f)	// right
        };
        float max = 0.0f;
        unsigned int best_match = -1;
        for (unsigned int i = 0; i < 4; i++) {
            float dot_product = glm::dot(glm::normalize(from.pos.xy() - to.pos.xy()), compass[i]);
            if (dot_product > max) {
                max = dot_product;
                best_match = i;
            }
        }
        return (Collision) best_match;
    } else {
        return NO_COLLISION;
    }
}

void MarioParticleSystem::handleCollision() {
    Particle& mario = mParticles[0];

    Particle& goomba = mParticles[1];
    Collision collision = collide(mario, goomba);
    if (collision == TOP) {
        mario.vel.y *= -1;
    } else if (collision != NO_COLLISION) {
        mario.pos = spawn;
        mario.vel = glm::vec3(0);
    }

    mario.onBrick = false;
    for (int i = 0; i < 10; ++i) {
        Particle& brick = mParticles[2 + i];
        collision = collide(mario, brick);
        if (collision == TOP) {
            mario.onBrick = true;
            mario.baseY = brick.pos.y + brick.size;
            break;
        }
        if (collision == BOTTOM) {
            mario.vel.y *= -1;
            mario.pos.y = brick.pos.y - brick.size;
        } else if (collision == LEFT || collision == RIGHT) {
            mario.vel.x = 0;
        }
    }
}