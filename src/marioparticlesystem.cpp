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

    particles["mario"].push_back(Mario(SPAWN));

    particles["goomba"].push_back(Goomba(glm::vec3(0.6, -0.6, 0)));

    particles["block"].push_back(Block(glm::vec3(-0.7, -0.3, 0), "brick"));
    particles["block"].push_back(Block(glm::vec3(-0.7, 0.25, 0), "question-0"));
    particles["block"].push_back(Block(glm::vec3(-0.5, -0.3, 0), "brick"));
    particles["block"].push_back(Block(glm::vec3(-0.3, -0.3, 0), "brick"));

    for (int i = 0; i < 5; ++i) {
        particles["block"].push_back(Block(glm::vec3(-1 + i * 0.2, -0.8, 0), "baseWall"));
        particles["block"].push_back(Block(glm::vec3(0.5 + i * 0.2, -0.8, 0), "baseWall"));
    }
}

void MarioParticleSystem::update(float dt) {
    handleCollision();
    updateMario(dt);
    updateGoomba(dt);
    updateMushroom(dt);
}

void MarioParticleSystem::updateMario(float dt) {
    Particle& mario = particles["mario"][0];

    // jump
    if (pressedKeys.find(GLFW_KEY_UP) != pressedKeys.end() && pressedKeys[GLFW_KEY_UP] &&
            mario.vel.y >= 0 && mario.pos.y - mario.baseY < 0.4) {
        mario.vel.y = 2;
        mario.onBlock = false;
    }

    // left and right
    if (pressedKeys.find(GLFW_KEY_LEFT) != pressedKeys.end() && pressedKeys[GLFW_KEY_LEFT]) {
        mario.force.x = -1;
        if (mario.pos.y == mario.baseY) {
            mario.left = true;
        }
    } else if (pressedKeys.find(GLFW_KEY_RIGHT) != pressedKeys.end() && pressedKeys[GLFW_KEY_RIGHT]) {
        mario.force.x = 1;
        if (mario.pos.y == mario.baseY) {
            mario.left = false;
        }
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
    if (mario.onBlock) {
        mario.pos.y = mario.baseY;
        mario.vel.y = glm::max(0.0f, mario.vel.y);
    }

    // death
    if (mario.pos.y < LOWER_Y) {
        mario.vel = glm::vec3(0);
        mario.pos = SPAWN;
    }

    // texture
    mario.texture = "mario";
    if (mario.left) {
        mario.texture += "-left";
    }
    if (mario.large) {
        mario.texture += "-l";
    }
    if (mario.pos.y > mario.baseY) {
        mario.texture += "-jump";
    } else {
        mario.texture += "-0";
    }
}

void MarioParticleSystem::updateGoomba(float dt) {
    for (auto& goomba : particles.at("goomba")) {
        if (!goomba.died) {
            goomba.pos = goomba.pos + dt * goomba.vel;
            if (glm::abs(goomba.pos.x - goomba.initPos.x) > 0.15) {
                goomba.vel.x *= -1;
            }
        }
    }
}

void MarioParticleSystem::updateMushroom(float dt) {
    auto it = particles["mushrooms"].begin();
    while (it != particles["mushrooms"].end()) {
        auto& mushroom = *it;
        mushroom.pos = mushroom.pos + dt * mushroom.vel;
        mushroom.vel = mushroom.vel + dt * mushroom.force / mushroom.mass;
        if (mushroom.onBlock) {
            mushroom.pos.y = mushroom.baseY;
            mushroom.vel.y = glm::max(0.0f, mushroom.vel.y);
        }
        if (mushroom.pos.y < LOWER_Y) {
            it = particles["mushrooms"].erase(it);
        } else {
            ++it;
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
    // mario
    Particle& mario = particles["mario"][0];
    for (auto& goomba : particles["goomba"]) {
        if (!goomba.died) {
            Collision collision = collide(mario, goomba);
            if (collision == TOP) {
                mario.vel.y *= -0.8;
                goomba.texture = "goomba-died";
                goomba.died = true;
                goomba.eraseCounter = 30;
                break;
            } else if (collision != NO_COLLISION) {
                mario.pos = SPAWN;
                mario.vel = glm::vec3(0);
                break;
            }
        }
    }
    auto it = particles["mushrooms"].begin();
    while (it != particles["mushrooms"].end()) {
        Collision collision = collide(mario, *it);
        if (collision != NO_COLLISION) {
            particles["mushrooms"].erase(it);
            //mario.large = true;
            break;
        }
        ++it;
    }

    for (auto& mushroom : particles["mushrooms"]) {
        Collision collision = collide(mario, mushroom);
        if (collision != NO_COLLISION) {

        }
    }
    handleAllBlockCollision(mario, false);

    // mushrooms
    for (auto& mushroom : particles["mushrooms"]) {
        handleAllBlockCollision(mushroom, false);
    }

    // stars
    for (auto& mushroom : particles["stars"]) {
        handleAllBlockCollision(mushroom, true);
    }
}

void MarioParticleSystem::handleAllBlockCollision(Particle& object, bool bounce) {
    object.onBlock = false;
    for (auto& block : particles["block"]) {
        if (handleBlockCollision(object, block, bounce)) {
            break;
        }
    }
}

bool MarioParticleSystem::handleBlockCollision(Particle &object, Particle &block, bool bounce) {
    Collision collision = collide(object, block);
    if (collision == TOP) {
        object.onBlock = true;
        object.baseY = block.pos.y + block.size;
        if (bounce) {
            object.vel.y *= -1;
        }
        return true;
    }
    if (collision == BOTTOM) {
        object.vel.y *= -1;
        object.pos.y = block.pos.y - block.size;

        if (block.texture == "question-0") {
            particles["mushrooms"].push_back(Mushroom(glm::vec3(
                block.pos.x, block.pos.y + block.size + 0.01, block.pos.z
            )));
            block.texture = "question-1";
        }

    } else if (collision == LEFT || collision == RIGHT) {
        object.vel.x = 0;
    }
    return false;
}

void MarioParticleSystem::draw() {
    GLuint texId = -1;
    for (auto& kv : particles) {
        for (auto& p : kv.second) {
            if (p.eraseCounter > 0) {
                p.eraseCounter--;
            }
            if (p.eraseCounter != 0) {
                if (textures[p.texture] != texId) {
                    texId = textures[p.texture];
                    theRenderer.begin(texId, mBlendMode);
                }
                theRenderer.quad(p);
            }
        }
    }
}
