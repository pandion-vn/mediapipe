#ifndef CHAIN_H
#define CHAIN_H

#include <stdio.h>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include "target.h"
#include "segment.h"

const glm::vec3 ref_rot_vector(0.0f, 0.0f, -1.0f);

class Chain {
public:
    Chain(std::vector<glm::vec3> joints, Target* target);
    void Render(glm::mat4 view, glm::mat4 proj);
    void Solve();
    void Backward(); // Put second endpoint at target and work backwards
    void Forward();  // Put first endpoint at origin and work forwards
    glm::vec3 Constrain(glm::vec3 point, float true_length, Segment* seg);

    void CalculateLinks(std::vector<glm::vec3> joints, std::vector<float> * lengths, std::vector<glm::quat> * directions);
    glm::vec3 GetFirstJoint();
    void SetFirstJoint(glm::vec3 joint);
    void SetSegments();

    unsigned long size;
    float total_length;
    glm::vec3 origin;
    glm::vec3 end;
    Target *target;
    bool please_constrain = false;
private:
    std::vector<glm::vec3> joints; // Joints themselves
    std::vector<Segment> segments; // The pieces that actually get rendered
    float tolerance = 0.001f;
};

#endif