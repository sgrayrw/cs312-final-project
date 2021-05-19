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

    for (int i = 0; i < 50; ++i) {
        if (i == 11 || i == 12 || i == 19 || i == 20) {
            continue;
        }

        particles["scene"].push_back(Block({-1.37 + i * 0.2, -1.37, 0}, "baseWall"));
        particles["block"].push_back(Block({-1.37 + i * 0.2, -1.37 + 0.2, 0}, "baseWall"));
    }

    particles["scene"].push_back(Scene({-.5, 1, -1}, 1, .5, "cloud-3"));
    particles["scene"].push_back(Scene({1.5, .9, -1}, .5, .4, "cloud-1"));
    particles["scene"].push_back(Scene({4, .9, -1}, .6, .4, "cloud-2"));

    particles["scene"].push_back(Scene({-1, -1.2, -1}, .6, .3, "tree-1"));
    particles["scene"].push_back(Scene({-0.1, -1.2, -1}, .8, .35, "mountain"));
    particles["scene"].push_back(Scene({1.8, -1.2, -1}, .1, .35, "tree-0"));
    particles["scene"].push_back(Scene({1.92, -1.2, -1}, .1, .35, "tree-0"));
    particles["scene"].push_back(Scene({2.04, -1.2, -1}, .1, .35, "tree-0"));
    particles["scene"].push_back(Scene({3.7, -1.2, -1}, .9, .3, "tree-3"));

    particles["scene"].push_back(Scene({6.5, -0.6, -1}, 1.3, 1.5, "castle"));
    particles["scene"].push_back(Scene({6.5, 0.29, -1}, 0.25, 0.25, "castle-flag"));

    particles["scene"].push_back(Scene({5.2, -.28, -.01}, 0.25, 1.6, "flagpole"));

    particles["block"].push_back(Block({-0.8 + 0.3, -0.6, 0}, "brick"));
    particles["block"].push_back(Block({-0.6 + 0.3, -0.6, 0}, "brick"));
    particles["block"].push_back(Block({-0.4 + 0.3, -0.6, 0}, "brick"));
    particles["block"].push_back(Block({-0.6 + 0.3, -0, 0}, "question-0"));

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j <= i; ++j) {
            particles["block"].push_back(Block({4.03 + i * 0.2, -.97 + j * 0.2, 0}, "brick-2"));
        }
    }

    // hack to use "scene", other map entries will cause the texture to be loaded incorrectly, no idea why!
    particles["scene"].push_back(Goomba({0.33, -.97, 0}, 1));
    particles["scene"].push_back(Goomba({1.467, -.97, 0}, -1));
    particles["scene"].push_back(Goomba({3.53, -.97, 0}, 1));

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j <= i; ++j) {
            particles["block"].push_back(Block({4.03 + i * 0.2, -.97 + j * 0.2, 0}, "brick-2"));
        }
    }

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j <= i; ++j) {
            particles["block"].push_back(Block({2.03 + i * 0.2, -.97 + j * 0.2, 0}, "brick-2"));
        }
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j <= 1 - i; ++j) {
            particles["block"].push_back(Block({2.83 + i * 0.2, -.97 + j * 0.2, 0}, "brick-2"));
        }
    }
    particles["scene"].push_back(Coin({2.23, -0.3, 0}));
    particles["scene"].push_back(Coin({2.43, -0.2, 0}));
    particles["scene"].push_back(Coin({2.63, -0.2, 0}));
    particles["scene"].push_back(Coin({2.83, -0.3, 0}));

    particles["block"].push_back(Block({1.35, -0.2, 0}, "brick"));
    particles["block"].push_back(Block({1.55, -0.2, 0}, "brick"));
    particles["block"].push_back(Block({1.75, -0.2, 0}, "brick"));

    for (int i = 0; i < 5; ++i) {
        particles["scene"].push_back(Coin({2.3 + i * 0.2, 0.45, 0}));
        particles["block"].push_back(Block({2.3 + i * 0.2, 0.25, 0}, "brick"));
    }

}

void MarioParticleSystem::restart() {
    Particle& mario = particles["mario"][0];
    mario.vel = glm::vec3(0);
    mario.pos = SPAWN;
    mario.died = false;
    mario.large = false;

    for (auto& scene : particles["scene"]) {
        if (scene.texture == "coin") {
            scene.eraseCounter = -1;
        }

        if (scene.texture.find("goomba") == std::string::npos) {
            continue;
        }
        scene.eraseCounter = -1;
        scene.died = false;
        scene.vel.x = 0.2;
        scene.pos = scene.initPos;
        scene.texture = "goomba-0";
    }

    for (auto& block : particles["block"]) {
        if (block.texture == "question-1") {
            block.texture = "question-0";
        }
        block.eraseCounter = -1;
    }
}

void MarioParticleSystem::update(float dt) {
    handleCollision();
    updateMario(dt);
    updateGoomba(dt);
    updateMushroom(dt);
    updateBrick(dt);
}

void MarioParticleSystem::updateMario(float dt) {
    Particle& mario = particles["mario"][0];
    mario.runCounter++;

    if (mario.died) {
        mario.vel.x = 0;
        mario.pos = mario.pos + dt * mario.vel;
        mario.vel = mario.vel + dt * glm::vec3(0, -5, 0) / mario.mass;
        if (mario.pos.y < LOWER_Y) {
            restart();
        }
        return;
    }

    // jump
    if (pressedKeys.find(GLFW_KEY_UP) != pressedKeys.end() && pressedKeys[GLFW_KEY_UP] &&
            mario.vel.y >= 0 && mario.pos.y - mario.baseY < 0.5) {
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
    mario.pos.x = glm::clamp(mario.pos.x, -1.376f, 6.93f);
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
        restart();
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
    } else if (glm::sign(mario.vel.x) != glm::sign(mario.force.x) && glm::abs(mario.force.x) > 1) {
        mario.texture += "-b";
    } else if (glm::distance(mario.lastPos, mario.pos) < 1e-3) {
        mario.texture += "-0";
    } else if (glm::abs(mario.force.x) < 0.5) {
        mario.texture += "-" + std::to_string((mario.runCounter / 10) % 3 + 1);
    } else {
        mario.texture += "-" + std::to_string((mario.runCounter / 5) % 3 + 1);
    }

    mario.lastPos = mario.pos;
    theRenderer.lookAt(
            {glm::clamp(mario.pos.x, 0.0f, 5.57f), 0, 4},
            {glm::clamp(mario.pos.x, 0.0f, 5.57f), 0, 0}
    );
}

void MarioParticleSystem::updateGoomba(float dt) {
    for (auto& goomba : particles["scene"]) {
        if (goomba.texture.find("goomba") == std::string::npos) {
            continue;
        }
        goomba.runCounter++;
        if (!goomba.died) {
            goomba.pos = goomba.pos + dt * goomba.vel;
            if (glm::abs(goomba.pos.x - goomba.initPos.x) > 0.33) {
                goomba.vel.x *= -1;
            }
            goomba.texture = "goomba-" + std::to_string((goomba.runCounter / 10) % 2);
        }
    }
}

void MarioParticleSystem::updateBrick(float dt) {
    for (auto& block : particles["block"]) {
        if (block.texture == "brick") {
            block.pos = block.pos + dt * block.vel;
            block.vel = block.vel + dt * block.force / block.mass;
            if (block.pos.y <= block.baseY) {
                block.pos.y = block.baseY;
                block.vel.y = glm::max(0.0f, block.vel.y);
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
                glm::vec2(0.0f, 1.0f),    // up
                glm::vec2(0.0f, -1.0f),    // down
                glm::vec2(-1.0f, 0.0f),    // left
                glm::vec2(1.0f, 0.0f)    // right
        };
        float max = 0.0f;
        int best_match = -1;
        for (int i = 0; i < 4; ++i) {
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
    // mushrooms
    for (auto& mushroom : particles["mushrooms"]) {
        handleAllBlockCollision(mushroom, false);
    }

    // stars
    for (auto& mushroom : particles["stars"]) {
        handleAllBlockCollision(mushroom, true);
    }

    // mario
    Particle& mario = particles["mario"][0];
    if (mario.died) {
        return;
    }
    for (auto& goomba : particles["scene"]) {
        if (goomba.texture.find("goomba") == std::string::npos) {
            continue;
        }
        if (!goomba.died) {
            Collision collision = collide(mario, goomba);
            if (collision == TOP) {
                mario.vel.y *= -0.8;
                goomba.texture = "goomba-died";
                goomba.died = true;
                goomba.eraseCounter = 30;
                break;
            } else if (collision != NO_COLLISION && mario.resetCounter == 0) {
                if (mario.large) {
                    mario.large = false;
                    mario.resetCounter = 120;
                } else {
                    mario.died = true;
                    mario.texture = "mario-died";
                    mario.vel = glm::vec3(0, 2, 0);
                }
                break;
            }
        }
    }
    auto it = particles["mushrooms"].begin();
    while (it != particles["mushrooms"].end()) {
        Collision collision = collide(mario, *it);
        if (collision != NO_COLLISION) {
            particles["mushrooms"].erase(it);
            mario.large = true;
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
}

void MarioParticleSystem::handleAllBlockCollision(Particle& object, bool bounce) {
    object.onBlock = false;
    for (auto& block : particles["block"]) {
        Collision collision = handleBlockCollision(object, block, bounce);
        if (object.onBlock && (collision == RIGHT || collision == LEFT)) {
            break;
        }
    }

    if (object.texture.find("mario") == std::string::npos) {
        return;
    }

    for (auto& block : particles["scene"]) {
        if (block.texture != "coin") {
            continue;
        }
        Collision collision = handleBlockCollision(object, block, bounce);
        if (object.onBlock && (collision == RIGHT || collision == LEFT)) {
            break;
        }
    }
}

Collision MarioParticleSystem::handleBlockCollision(Particle &object, Particle &block, bool bounce) {
    Collision collision = collide(object, block);

    if (collision != NO_COLLISION && block.texture == "coin") {
        block.eraseCounter = 0;
        return collision;
    }

    if (collision == TOP) {
        object.onBlock = true;
        object.baseY = block.pos.y + block.size;
        if (bounce) {
            object.vel.y *= -1;
        }
        return collision;
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

        if (block.texture == "brick") {
            block.vel.y = 1;
        }

    } else if (collision == LEFT || collision == RIGHT) {
        object.vel.x = 0;
    }
    return collision;
}

void MarioParticleSystem::draw() {
    GLuint texId = -1;
    for (auto& kv : particles) {
        for (auto& p : kv.second) {
            if (p.resetCounter > 0) {
                p.resetCounter--;
            }
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
