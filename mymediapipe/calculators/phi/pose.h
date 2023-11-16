#ifndef POSE_H__
#define POSE_H__

#include "common.h"
#include "model.h"
#include "phi_shader.h"
#include "target.h"

class Pose {
public:
    std::vector<Target*> landmarks;

    Pose();
    ~Pose();
    void SetLandmarks(std::vector<glm::vec3> &lm3d);
    void SetColor(glm::vec3 color);
    void Render(const glm::mat4 &view, const glm::mat4 &proj);

};

inline Pose::Pose() {
    for (int i=0; i< 33; i++) {
        Target* target = new Target(0.0, 0.0, 0.0);
        target->SetColor(glm::vec3(1.0f, 1.0f, 0.0f));
        landmarks.push_back(target);
        // std::cout << "Pose default target: " << i << std::endl;
    }
}

inline Pose::~Pose() {
}

inline void Pose::SetLandmarks(std::vector<glm::vec3> &lm3d) {
    for (int i=0; i< lm3d.size(); i++) {
        landmarks[i]->SetPosition(lm3d[i]);
        // target.SetColor(glm::vec3(1.0f, 1.0f, 0.0f));
        // landmarks.push_back(target);
    }
    // std::cout << "Pose landmarks: " << landmarks.size() << std::endl;
}

inline void Pose::Render(const glm::mat4 &view, const glm::mat4 &proj) {
    for (int i=0; i< landmarks.size(); i++) {
        landmarks[i]->Render(view, proj);
    }
}

#endif // POSE_H__