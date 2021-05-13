#ifndef ParticleSystem_H_
#define ParticleSystem_H_

#include "util/AGL.h"
#include "util/image.h"
#include "renderer.h"
#include <vector>

namespace agl {

    struct Particle {
        Particle(glm::vec3 _pos, glm::vec3 _vel, glm::vec3 _force, glm::vec4 _color, float _size, glm::vec3 _mass)
                : pos(_pos), vel(_vel), force(_force), color(_color), size(_size), mass(_mass) {
            initPos = _pos;
        }

        glm::vec3 pos;
        glm::vec3 vel;
        glm::vec3 force;
        glm::vec4 color;
        float size;
        glm::vec3 mass;

        glm::vec3 initPos;
    };

    class ParticleSystem {
    public:
        ParticleSystem();

        virtual ~ParticleSystem();

        void init(int size);

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