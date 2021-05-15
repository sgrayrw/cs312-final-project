//
// Created by Yuxiao Wang on 5/15/21.
//

#ifndef PROJECT_PARTICLE_H
#define PROJECT_PARTICLE_H

struct Particle {
    Particle(glm::vec3 pos, glm::vec3 vel, glm::vec3 force, glm::vec4 color, float size, glm::vec3 mass, std::string texture)
            : pos(pos), vel(vel), force(force), color(color), size(size), mass(mass), texture(texture) {
        initPos = pos;
        baseY = pos.y;
    }

    Particle(glm::vec3 pos, glm::vec3 vel, std::string texture) : Particle(
            pos,
            vel,
            glm::vec3(0),
            glm::vec4(1),
            0.2,
            glm::vec3(1),
            texture
    ) {}

    Particle(glm::vec3 pos, std::string texture) : Particle(pos, glm::vec3(0), texture) {}

    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 force;
    glm::vec4 color;
    float size;
    glm::vec3 mass;

    glm::vec3 initPos;
    float baseY;
    bool onBlock = false;

    std::string texture;
    bool large = false;
    bool died = false;
    int eraseCounter = -1;
    bool left = false;
};

#endif //PROJECT_PARTICLE_H
