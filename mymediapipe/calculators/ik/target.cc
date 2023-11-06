#include "target.h"

Target::Target(int x, int y, int z) {
    // Add a bit of noise to the target, because if the target
    // starts in a perfect location, the joints might overlap which
    // messes up the algorithm
    position = glm::vec3(x, y, z) + 0.0001f;
    scale = glm::vec3 (.05f, .05f, .05f);
    pitch = 0.0f;
    yaw = 0.0f;
}