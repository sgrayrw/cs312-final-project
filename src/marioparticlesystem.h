#ifndef MyParticleSystem_H_
#define MyParticleSystem_H_

#include "particlesystem.h"

namespace agl {

    class MarioParticleSystem : public ParticleSystem {
    public:
        void createParticles(int size) override;

        void update(float dt) override;

        void setKey(int key, int action);

    private:
        int lastPressedKey = -1;
        int lastKeyAction = -1;
    };
}
#endif