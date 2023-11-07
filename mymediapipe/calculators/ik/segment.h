#ifndef SEGMENT_H
#define SEGMENT_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
// #include <glm/gtc/noise.hpp>
// #include <glm/gtx/rotate_vector.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtx/transform.hpp>

#include "camera.h"
#include "mymediapipe/calculators/gpu/shader.h"

class Segment {
public:
    glm::vec3 position;
    glm::vec3 end_position;
    glm::quat quat;
    float magnitude;

    Segment(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir);
    void Render(glm::mat4 view, glm::mat4 proj);

    // The constraint cone, symbolized by the degrees going in the up, down, left, right directions
    glm::vec4 constraint_cone;
    void SetConstraintConeDegrees(glm::vec4 degrees);

    void ProcessTranslation(Camera_Movement direction, GLfloat deltaTime);
    void Set(glm::vec3 base, glm::vec3 end, float magnitude, glm::quat dir);
    
    // 0, 1, 2, 3 = Up, Down, Left, Right. Make sure you wrap each index around a vec3
    glm::mat4 GetFaceNormals();
    glm::vec3 GetConstraintConeAxis();
private:
    /* Data */
    // GLchar* vertexShaderPath = "mymediapipe/assets/ik/seg.vs";
    // GLchar* fragShaderPath   = "mymediapipe/assets/ik/seg.frag";
    const GLchar* vertexShaderPath = R"(
        #version 310 es
        precision highp float;
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 normal;

        out vec3 Normal;
        out vec3 FragPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view *  model * vec4(position, 1.0f);
            FragPos = vec3(model * vec4(position, 1.0f));
            Normal = mat3(transpose(inverse(model))) * normal;  
        }
    )";

    const GLchar* fragShaderPath = R"(
        #version 310 es
        precision highp float;
        out vec4 fragColor;

        in vec3 FragPos;  
        in vec3 Normal;  
        
        uniform vec3 lightPos; 
        uniform vec3 viewPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;

        void main()
        {
            // Ambient
            float ambientStrength = 0.1f;
            vec3 ambient = ambientStrength * lightColor;
            
            // Diffuse 
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
            // Specular
            float specularStrength = 0.5f;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
            vec3 specular = specularStrength * spec * lightColor;  
                
            vec3 result = (ambient + diffuse + specular) * objectColor;
            fragColor = vec4(result, 1.0f);
        } 
    )";
    Shader objectShader;
};

#endif