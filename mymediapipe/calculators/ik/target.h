#ifndef TARGET_H
#define TARGET_H

#include "glm/glm.hpp"

class Target {
public:
    glm::vec3 position;
    float pitch;
    float yaw;
    glm::vec3 scale;

    Target(int x, int y, int z);
};
#endif