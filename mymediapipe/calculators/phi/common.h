#ifndef COMMON_H__
#define COMMON_H__

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "mediapipe/gpu/gl_base.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
// #include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// #define TVecCoord std::vector<glm::vec3>
#define TCoord glm::vec3

#endif