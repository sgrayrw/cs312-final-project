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
                glm::vec3(0.2, 1, 1)
        ) {}
    };

    struct Goomba : Particle {
        Goomba(glm::vec3 pos) : Particle(pos, glm::vec3(0.1, 0, 0)) {}
    };

    struct BaseWall : Particle {
        BaseWall(glm::vec3 pos) : Particle(pos) {}
    };

    struct Brick : Particle {
        Brick(glm::vec3 pos) : Particle(pos) {}
    };

    struct Background : Particle {
        Background(glm::vec3 color) : Particle(
           glm::vec3(0, 0, -5),
           glm::vec3(0),
           glm::vec3(0),
           glm::vec4(color, 1),
           100,
           glm::vec3(0)
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
        const glm::vec3 spawn{-0.9, 1, 0};
        std::unordered_map<int, bool> pressedKeys;
        std::unordered_map<std::string, GLuint> textures;
        std::unordered_map<std::string, std::vector<Particle>> particles;

        void loadTextures();
        void updateMario(float dt);
        void updateGoomba(float dt);
        Collision collide(const Particle &from, const Particle &to);
        void handleCollision();
        bool handleBrickCollision(Particle &mario, Particle &baseWall);
    };
}
#endif