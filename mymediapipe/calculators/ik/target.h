#ifndef TARGET_H
#define TARGET_H

#include "mymediapipe/calculators/gpu/shader.h"
#include "model.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Target {
public:
    glm::vec3 position;
    float pitch;
    float yaw;
    glm::vec3 scale;

    Target(int x, int y, int z);
    void Render(glm::mat4 view, glm::mat4 proj);
private:
  
    /* Data */
    Model objectModel;
    Shader objectShader;
    GLchar* pathToModel      = "mymediapipe/assets/ik/sphere.off";
    GLchar* vertexShaderPath = R"(
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
    // "/Users/jezhou/Git/classes/s17/ik-opengl/shader.vs";
    GLchar* fragShaderPath   = R"(
        #version 310 es
        precision highp float;
        out vec4 color;

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
            color = vec4(result, 1.0f);
        } 

    )";
    // "/Users/jezhou/Git/classes/s17/ik-opengl/shader.frag";
};
#endif