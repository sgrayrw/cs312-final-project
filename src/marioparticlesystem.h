#ifndef MyParticleSystem_H_
#define MyParticleSystem_H_

#include <unordered_map>
#include "particlesystem.h"

namespace agl {
    enum Collision {
        NO_COLLISION = -1, TOP, BOTTOM, LEFT, RIGHT
    };

    struct Mario : Particle {
        Mario(glm::vec3 pos) : Particle(
                pos,
                glm::vec3(0),
                glm::vec3(0, -9.8, 0), // gravity
                glm::vec4(1),
                0.2,
                glm::vec3(0.2, 1, 1),
                "mario-0"
        ) {}
    };

    struct Goomba : Particle {
        Goomba(glm::vec3 pos) : Particle(pos, glm::vec3(0.1, 0, 0), "goomba-0") {}
    };

    struct Block : Particle {
        Block(glm::vec3 pos, std::string texture) : Particle(
                pos,
                glm::vec3(0),
                glm::vec3(0, -9.8, 0), // gravity
                glm::vec4(1),
                0.2,
                glm::vec3(0.2, 1, 1),
                texture
        ) {}
    };

    struct Coin : Particle {
        Coin(glm::vec3 pos) : Particle(
                pos,
                glm::vec3(0),
        glm::vec3(0, -9.8, 0), // gravity
        glm::vec4(1),
        0.1,
        glm::vec3(0.2, 1, 1),
        "coin"
        ) {}
    };

    struct Mushroom : Particle {
        Mushroom(glm::vec3 pos) : Particle(
                pos,
                glm::vec3(0.35, 2, 0),
                glm::vec3(0, -9.8, 0), // gravity
                glm::vec4(1),
                0.2,
                glm::vec3(0.2, 1, 1),
                "mushroom"
        ) {}
    };

    struct Background : Particle {
        Background(glm::vec3 color) : Particle(
               glm::vec3(0, 0, -2),
               glm::vec3(0),
               glm::vec3(0),
               glm::vec4(color, 1),
               1000,
               glm::vec3(0),
               "background"
        ) {}
    };

    struct Scene : Particle {
        Scene(glm::vec3 pos, float sizeX, float sizeY, std::string texture) : Particle(
                pos,
                glm::vec3(0),
                glm::vec3(0),
                glm::vec4(1),
                sizeX,
                sizeY,
                glm::vec3(0),
                texture
        ) {}
    };

    class MarioParticleSystem : public ParticleSystem {
    public:
        void init(int size) override;
        void createParticles(int size) override;
        void update(float dt) override;
        void draw() override;
        void setKey(int key, int action);

    private:
        const glm::vec3 SPAWN{-1.3, 1, 0};
        const float LOWER_Y = -3;

        std::unordered_map<int, bool> pressedKeys;
        std::unordered_map<std::string, GLuint> textures;
        std::unordered_map<std::string, std::vector<Particle>> particles;

        void restart();
        void loadTextures();
        void updateMario(float dt);
        void updateGoomba(float dt);
        void updateBrick(float dt);
        void updateMushroom(float dt);

        Collision collide(const Particle &from, const Particle &to);
        void handleCollision();
        void handleAllBlockCollision(Particle &object, bool bounce);
        bool handleBlockCollision(Particle &object, Particle &block, bool bounce);
    };
}
#endif