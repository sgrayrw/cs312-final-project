#ifndef MyParticleSystem_H_
#define MyParticleSystem_H_

#include <unordered_map>
#include "particlesystem.h"

namespace agl {
    enum Collision {
        NO_COLLISION = -1, TOP, BOTTOM, LEFT, RIGHT
    };

    class MarioParticleSystem : public ParticleSystem {
    public:
        void createParticles(int size) override;
        void update(float dt) override;
        void setKey(int key, int action);

    private:
        const glm::vec3 spawn = glm::vec3(-0.9, 0, 0);
        std::unordered_map<int, bool> pressedKeys;

        void updateMario(float dt);
        void updateGoomba(float dt);
        Collision collide(const Particle &from, const Particle &to);
        void handleCollision();
    };
}
#endif