#ifndef SEGMENT_H
#define SEGMENT_H

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include "shader.h"

class Segment {
public:
    glm::vec3 position;
    glm::vec3 end_position;
    glm::quat quat;
    float magnitude;

    Segment(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir);
    void Render(glm::mat4 view, glm::mat4 proj);
private:
    /* Data */
    GLchar* vertexShaderPath = "seg.vs";
    GLchar* fragShaderPath   = "seg.frag";
    Shader objectShader;
};

#endif