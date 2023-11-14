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

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
// #include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#define TVecCoord std::vector<glm::vec3>
#define TCoord glm::vec3

#define VIEWPORT_WIDTH  1440
#define VIEWPORT_HEIGHT 900
#define VIEWPORT_RATIO (float)VIEWPORT_WIDTH/(float)VIEWPORT_HEIGHT

#define FLOOR_MODEL "mymediapipe/assets/phi/floor/floor.obj"
#define RAPTOR_MODEL "mymediapipe/assets/phi/raptor/raptor.dae"
#define DART_MAUL_MODEL "mymediapipe/assets/phi/dartmaul/dartmaul.dae"

#endif