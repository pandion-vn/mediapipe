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

    // glm::vec3 hip_landmarks[36];
    // glm::vec3 a = glm::vec3(1.0f, 0.0f, 0.0f);
    // glm::vec3 b = glm::vec3(1.0f, 1.0f, 0.0f);
    // glm::vec3 center = (a + b) * 0.5f;
    // std::cout << "center point: " << glm::to_string(center) << std::endl;
    // glm::vec3 subtract = b - center;
    // std::cout << "subtract: " << glm::to_string(subtract) << std::endl;
    // glm::vec3 normalize = glm::normalize(subtract);
    // std::cout << "normalize: " << glm::to_string(normalize) << std::endl;

    // std::vector<glm::vec3> landmarks = {
    //     glm::vec3( 0.0119763   ,-0.623208   ,-0.2408),
    //     glm::vec3( 0.0207183   ,-0.661144   ,-0.225762),
    //     glm::vec3( 0.0210193   ,-0.661992   ,-0.224812),
    //     glm::vec3( 0.0204283   ,-0.661951   ,-0.225638),
    //     glm::vec3( -0.0145622  ,-0.658234   ,-0.228062),
    //     glm::vec3( -0.0144342  ,-0.658482   ,-0.22908),
    //     glm::vec3( -0.0148197  ,-0.659787   ,-0.228995),
    //     glm::vec3( 0.0678002   ,-0.658406   ,-0.111275),
    //     glm::vec3( -0.091814   ,-0.647906   ,-0.132005),
    //     glm::vec3( 0.0317771   ,-0.605163   ,-0.197224),
    //     glm::vec3( -0.0148868 ,-0.599234   ,-0.202294),
    //     glm::vec3( 0.16192    ,-0.500922   ,-0.0211353),
    //     glm::vec3( -0.180168  ,-0.483003   ,-0.0781416),
    //     glm::vec3( 0.317788   ,-0.42234    ,-0.00194242),
    //     glm::vec3( -0.368888  ,-0.383107   ,-0.0966824),
    //     glm::vec3( 0.544815   ,-0.445803   ,-0.0803205),
    //     glm::vec3( -0.554773  ,-0.370483   ,-0.212996),
    //     glm::vec3( 0.600138   ,-0.431338   ,-0.0916604),
    //     glm::vec3( -0.602189  ,-0.361586   ,-0.21734),
    //     glm::vec3( 0.609016   ,-0.441415   ,-0.1208),
    //     glm::vec3( -0.612772  ,-0.375528   ,-0.257249),
    //     glm::vec3( 0.557677   ,-0.444691   ,-0.0936855),
    //     glm::vec3( -0.565056  ,-0.371071   ,-0.235228),
    //     glm::vec3( 0.114538   ,-0.00226181 ,0.0217715),
    //     glm::vec3( -0.115572  ,0.00174514  ,-0.0210466),
    //     glm::vec3( 0.0889377  ,0.363915    ,0.00107452),
    //     glm::vec3( -0.106823  ,0.354405    ,-0.00320578),
    //     glm::vec3( 0.110436   ,0.643422    ,0.173277),
    //     glm::vec3( -0.0934725 ,0.640668    ,0.171273),
    //     glm::vec3( 0.117252   ,0.679806    ,0.183243),
    //     glm::vec3( -0.0923595 ,0.678795    ,0.186593),
    //     glm::vec3( 0.124085   ,0.717742    ,0.0787252),
    //     glm::vec3( -0.0913644 ,0.698742    ,0.0695883)
    // };

    std::vector<glm::vec3> landmarks = {
        glm::vec3(0.008416, -0.634527, -0.260950),
        glm::vec3(0.023932, -0.649963, -0.246623),
        glm::vec3(0.021935, -0.652791, -0.236793),
        glm::vec3(0.020602, -0.652721, -0.238632),
        glm::vec3(-0.001918, -0.657420, -0.264558),
        glm::vec3(-0.000328, -0.658103, -0.277128),
        glm::vec3(0.003014, -0.644046, -0.256564),
        glm::vec3(0.074619, -0.637829, -0.145254),
        glm::vec3(-0.067449, -0.582788, -0.155069),
        glm::vec3(0.042210, -0.618381, -0.213900),
        glm::vec3(0.003060, -0.583580, -0.246246),
        glm::vec3(0.167898, -0.529274, -0.047496),
        glm::vec3(-0.118491, -0.578913, -0.044215),
        glm::vec3(0.295250, -0.692503, -0.095195),
        glm::vec3(-0.226583, -0.653058, -0.189032),
        glm::vec3(0.388551, -0.875161, -0.130593),
        glm::vec3(-0.324218, -0.868572, -0.289152),
        glm::vec3(0.396092, -0.923208, -0.140909),
        glm::vec3(-0.326189, -0.924808, -0.353246),
        glm::vec3(0.378265, -0.947431, -0.157319),
        glm::vec3(-0.294475, -0.980209, -0.352735),
        glm::vec3(0.389180, -0.872154, -0.144568),
        glm::vec3(-0.318777, -0.897202, -0.299864),
        glm::vec3(0.084799, -0.005909, 0.027898),
        glm::vec3(-0.083190, 0.001052, -0.025328),
        glm::vec3(0.078926, 0.383440, 0.000040),
        glm::vec3(-0.046097, 0.336393, -0.012527),
        glm::vec3(0.061945, 0.736835, 0.103868),
        glm::vec3(-0.085045, 0.703524, 0.142972),
        glm::vec3(0.035896, 0.770580, 0.119718),
        glm::vec3(-0.089242, 0.737073, 0.120542),
        glm::vec3(0.031568, 0.807700, 0.066122),
        glm::vec3(-0.126434, 0.764138, 0.051662),
        glm::vec3(0.000804, -0.002429, 0.001285),
        glm::vec3(0.024704, -0.554094, -0.045855),
        glm::vec3(0.000804, -0.002429, 0.001285)
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
