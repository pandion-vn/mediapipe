#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include "Eigen/Core"
#include "absl/memory/memory.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "my_pose.h"

int main(int argc, char** argv) {
    std::cout << "test math" << std::endl;

    glm::vec3 hip_landmarks[36];
    glm::vec3 a = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 b = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 center = (a + b) * 0.5f;
    std::cout << "center point: " << glm::to_string(center) << std::endl;
    glm::vec3 subtract = b - center;
    std::cout << "subtract: " << glm::to_string(subtract) << std::endl;
    glm::vec3 normalize = glm::normalize(subtract);
    std::cout << "normalize: " << glm::to_string(normalize) << std::endl;

    std::vector<glm::vec3> landmarks = {
        glm::vec3( 0.0119763   ,-0.623208   ,-0.2408),
        glm::vec3( 0.0207183   ,-0.661144   ,-0.225762),
        glm::vec3( 0.0210193   ,-0.661992   ,-0.224812),
        glm::vec3( 0.0204283   ,-0.661951   ,-0.225638),
        glm::vec3( -0.0145622  ,-0.658234   ,-0.228062),
        glm::vec3( -0.0144342  ,-0.658482   ,-0.22908),
        glm::vec3( -0.0148197  ,-0.659787   ,-0.228995),
        glm::vec3( 0.0678002   ,-0.658406   ,-0.111275),
        glm::vec3( -0.091814   ,-0.647906   ,-0.132005),
        glm::vec3( 0.0317771   ,-0.605163   ,-0.197224),
        glm::vec3( -0.0148868 ,-0.599234   ,-0.202294),
        glm::vec3( 0.16192    ,-0.500922   ,-0.0211353),
        glm::vec3( -0.180168  ,-0.483003   ,-0.0781416),
        glm::vec3( 0.317788   ,-0.42234    ,-0.00194242),
        glm::vec3( -0.368888  ,-0.383107   ,-0.0966824),
        glm::vec3( 0.544815   ,-0.445803   ,-0.0803205),
        glm::vec3( -0.554773  ,-0.370483   ,-0.212996),
        glm::vec3( 0.600138   ,-0.431338   ,-0.0916604),
        glm::vec3( -0.602189  ,-0.361586   ,-0.21734),
        glm::vec3( 0.609016   ,-0.441415   ,-0.1208),
        glm::vec3( -0.612772  ,-0.375528   ,-0.257249),
        glm::vec3( 0.557677   ,-0.444691   ,-0.0936855),
        glm::vec3( -0.565056  ,-0.371071   ,-0.235228),
        glm::vec3( 0.114538   ,-0.00226181 ,0.0217715),
        glm::vec3( -0.115572  ,0.00174514  ,-0.0210466),
        glm::vec3( 0.0889377  ,0.363915    ,0.00107452),
        glm::vec3( -0.106823  ,0.354405    ,-0.00320578),
        glm::vec3( 0.110436   ,0.643422    ,0.173277),
        glm::vec3( -0.0934725 ,0.640668    ,0.171273),
        glm::vec3( 0.117252   ,0.679806    ,0.183243),
        glm::vec3( -0.0923595 ,0.678795    ,0.186593),
        glm::vec3( 0.124085   ,0.717742    ,0.0787252),
        glm::vec3( -0.0913644 ,0.698742    ,0.0695883)
    };

    for (int i=0; i<landmarks.size(); i++) {
        std::cout << "landmark point (" << i << "): " << glm::to_string(landmarks[i]) << std::endl;
    }

    auto rotations = pose_rotation(landmarks);

    std::cout << "set origin point: " << std::endl;
    for (int i=0; i<landmarks.size(); i++) {
        std::cout << "landmark point (" << i << "): " << glm::to_string(landmarks[i]) << std::endl;
    }

    std::cout << "rotations: " << std::endl;
    for (int i=0; i<rotations.size(); i++) {
        std::cout << "rotation point (" << i << "): " << glm::to_string(rotations[i]) << std::endl;

    }

    return EXIT_SUCCESS;
}
