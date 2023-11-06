#include "chain.h"

Chain::Chain(std::vector<glm::vec3> joints, Target* t) {
}

void Chain::Solve() {
}

void Chain::SetSegments() {
}

void Chain::Backward() {
}

void Chain::Forward() {
}

void Chain::Render(glm::mat4 view, glm::mat4 proj) {
    for(auto it = segments.begin(); it != segments.end(); ++it) {
        it->Render(view, proj);
    }
}