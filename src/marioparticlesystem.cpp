#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <filesystem>
#include "util/AGL.h"
#include "marioparticlesystem.h"

using namespace agl;

void MarioParticleSystem::init(int size) {
    if (!theRenderer.initialized()) {
        theRenderer.init("../shaders/billboard.vs", "../shaders/billboard.fs");
    }
    loadTextures();
    createParticles(size);
}

void MarioParticleSystem::loadTextures() {
    std::string textureDir = "../textures";
    for (const auto& entry: std::filesystem::directory_iterator(textureDir)) {
        std::string filename = entry.path().filename();
        filename = filename.substr(0, filename.size() - 4);
        textures[filename] = theRenderer.loadTexture(entry.path());
    }
}

void MarioParticleSystem::createParticles(int size) {
    particles["background"].push_back(Background(glm::vec3(1)));

    particles["mario"].push_back(Mario(spawn));

    particles["goomba"].push_back(Goomba(glm::vec3(0.5, 0, 0)));

    particles["brick"].push_back(Brick(glm::vec3(-0.2, 0.3, 0)));

    for (int i = 0; i < 3; ++i) {
        particles["baseWall"].push_back(BaseWall(glm::vec3(-1 + i * 0.2, -0.2, 0)));
        particles["baseWall"].push_back(BaseWall(glm::vec3(i * 0.2, -0.2, 0)));
    }
}

void MarioParticleSystem::update(float dt) {
    updateMario(dt);
    updateGoomba(dt);
    handleCollision();
}

void MarioParticleSystem::updateMario(float dt) {
    Particle& mario = particles.at("mario")[0];

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

    // clamp speed
    mario.vel.x = glm::clamp(mario.vel.x, -1.0f, 1.0f);

    // base
    if (mario.onBrick) {
        mario.pos.y = mario.baseY;
        mario.vel.y = glm::max(0.0f, mario.vel.y);
    }

    // death
    if (mario.pos.y < -1) {
        mario.vel = glm::vec3(0);
        mario.pos = spawn;
    }
}

void MarioParticleSystem::updateGoomba(float dt) {
    for (auto& goomba : particles.at("goomba")) {
        goomba.pos = goomba.pos + dt * goomba.vel;
        if (glm::abs(goomba.pos.x - goomba.initPos.x) > 0.15) {
            goomba.vel.x *= -1;
        }
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
    Particle& mario = particles.at("mario")[0];

    for (auto& goomba : particles.at("goomba")) {
        Collision collision = collide(mario, goomba);
        if (collision == TOP) {
            mario.vel.y *= -1;
            break;
        } else if (collision != NO_COLLISION) {
            mario.pos = spawn;
            mario.vel = glm::vec3(0);
            break;
        }
    }

    mario.onBrick = false;
    for (auto& brick : particles.at("brick")) {
        if (handleBrickCollision(mario, brick)) {
            break;
        }
    }
    if (!mario.onBrick) {
        for (auto& baseWall : particles.at("baseWall")) {
            if (handleBrickCollision(mario, baseWall)) {
                break;
            }
        }
    }
}

bool MarioParticleSystem::handleBrickCollision(Particle& mario, Particle& brick) {
    Collision collision = collide(mario, brick);
    if (collision == TOP) {
        mario.onBrick = true;
        mario.baseY = brick.pos.y + brick.size;
        return true;
    }
    if (collision == BOTTOM) {
        mario.vel.y *= -1;
        mario.pos.y = brick.pos.y - brick.size;
    } else if (collision == LEFT || collision == RIGHT) {
        mario.vel.x = 0;
    }
    return false;
}

void MarioParticleSystem::draw() {
    theRenderer.begin(textures.at("background"), mBlendMode);
    Particle& background = particles.at("background")[0];
    theRenderer.quad(background.pos, background.color, background.size);

    Particle& mario = particles.at("mario")[0];
    GLuint marioTex = (mario.pos.y > mario.baseY) ? textures.at("mario-jump") : textures.at("mario-0");
    theRenderer.begin(marioTex, mBlendMode);
    theRenderer.quad(mario.pos, mario.color, mario.size);

    theRenderer.begin(textures.at("goomba-0"), mBlendMode);
    for (auto& goomba : particles.at("goomba")) {
        theRenderer.quad(goomba.pos, goomba.color, goomba.size);
    }

    theRenderer.begin(textures.at("brick"), mBlendMode);
    for (auto& brick : particles.at("brick")) {
        theRenderer.quad(brick.pos, brick.color, brick.size);
    }

    theRenderer.begin(textures.at("baseWall"), mBlendMode);
    for (auto& baseWall : particles.at("baseWall")) {
        theRenderer.quad(baseWall.pos, baseWall.color, baseWall.size);
    }
}
