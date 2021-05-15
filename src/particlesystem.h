#ifndef ParticleSystem_H_
#define ParticleSystem_H_

#include "util/AGL.h"
#include "util/image.h"
#include "particle.h"
#include "renderer.h"
#include <vector>

namespace agl {
    class ParticleSystem {
    public:
        ParticleSystem();

        virtual ~ParticleSystem();

        virtual void init(int size);

        virtual void update(float dt) = 0;

        virtual void draw();

        static Renderer &GetRenderer();

    protected:
        virtual void createParticles(int size) = 0;

    protected:

        std::vector<Particle> mParticles;
        GLuint mTexture;
        BlendMode mBlendMode;
        static Renderer theRenderer;
    };
}
#endif