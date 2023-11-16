#ifndef TARGET_H__
#define TARGET_H__

#include "common.h"
#include "model.h"
#include "phi_shader.h"

class Target {
public:
    glm::vec3 position;
    float pitch;
    float yaw;
    glm::vec3 scale;
    glm::vec3 objectColor;

    Target(float x, float y, float z);
    ~Target();
    void SetPosition(glm::vec3 pos);
    void SetColor(glm::vec3 color);
    void Render(glm::mat4 view, glm::mat4 proj);
private:
    /* Data */
    Model *objectModel;
    PhiShader *objectShader;
    GLchar* pathToModel = "mymediapipe/assets/ik/sphere.off";
    GLchar* fragShaderPath   = "mymediapipe/assets/phi/shaders/target.fs";
    GLchar* vertexShaderPath = "mymediapipe/assets/phi/shaders/target.vs";
};

inline Target::Target(float x, float y, float z) {
    // Add a bit of noise to the target, because if the target
    // starts in a perfect location, the joints might overlap which
    // messes up the algorithm
    position = glm::vec3(x, y, z) + 0.0001f;
    scale = glm::vec3 (.05f, .05f, .05f);
    pitch = 0.0f;
    yaw = 0.0f;

    objectColor = glm::vec3(1.0f, 1.0f, 1.0f);

    // Create the shader to use for the controller
    objectShader = new PhiShader(vertexShaderPath, fragShaderPath);

    // Creates the model for the controller
    objectModel = new Model(pathToModel);
}

inline Target::~Target() {
    delete objectModel;
    delete objectShader;
}

inline void Target::SetPosition(glm::vec3 pos) {
    position = pos;
}

inline void Target::SetColor(glm::vec3 color) {
    objectColor = color;
}

inline void Target::Render(glm::mat4 view, glm::mat4 proj) {
    objectShader->Use();
    
    objectShader->SetUniform("objectColor", objectColor);
    objectShader->SetUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    objectShader->SetUniform("lightPos", glm::vec3(1.0f, 1.0f, 3.0f));
    objectShader->SetUniform("viewPos", glm::vec3(0.0f, 0.0f, 3.0f));
    
    // Calculate the toWorld matrix for the model
    glm::mat4 model;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1, 0, 0));
    R = glm::rotate(R, yaw, glm::vec3(0, 0, 1));
    model = T * R * S;
    
    objectShader->SetUniform("model", model);
    objectShader->SetUniform("view", view);
    objectShader->SetUniform("projection", proj);
    // glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    // glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    // glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    
    objectModel->Draw(*objectShader);
}

#endif // TARGET_H__