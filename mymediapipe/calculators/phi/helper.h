#ifndef HELPER_H__
#define HELPER_H__

#include "common.h"
#include "glm/gtc/quaternion.hpp"

glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from);
glm::vec3 aiColor4DToGlm(const aiColor4D& from);
glm::vec3 decomposeT(const glm::mat4& m);
glm::mat4 decomposeR(const glm::mat4& m);
void decomposeTRS(const glm::mat4& m, glm::vec3& scaling, glm::mat4& rotation, glm::vec3& translation);

#endif