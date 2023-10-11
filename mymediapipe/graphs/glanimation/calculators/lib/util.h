#ifndef UTIL_H
#define UTIL_H

#include "mediapipe/gpu/gl_base.h"
#include "mediapipe/gpu/shader_util.h"
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/quaternion.h"
#include "assimp/vector3.h"
#include "assimp/matrix4x4.h"

GLuint loadTexture(char const *path, bool gamma=false);
GLuint TextureFromEmbedded(const aiTexture* paiTexture);
GLuint TextureFromFile(const char *path, const std::string &directory, bool gamma=false);
glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
glm::vec2 GetGLMVec2(const aiVector3D& vec);
glm::vec3 GetGLMVec3(const aiVector3D& vec);
glm::quat GetGLMQuat(const aiQuaternion& pOrientation);

#endif