#ifndef FLOOR_H__
#define FLOOR_H__

#include "common.h"
#include "model.h"
#include "phi_shader.h"

class Floor {
public:
    Floor();
    ~Floor();
    // void SetPosition(glm::vec3 pos);
    // void SetColor(glm::vec3 color);
    void Render(Light *light, const glm::mat4 &view, const glm::mat4 &proj);
    PhiShader *objectShader;
private:
    /* Data */
    Model *objectModel;
    GLchar* pathToModel = "mymediapipe/assets/ik/sphere.off";
    GLchar* fragShaderPath   = "mymediapipe/assets/phi/shaders/fem.fs";
    GLchar* vertexShaderPath = "mymediapipe/assets/phi/shaders/fem.vs";
};

inline Floor::Floor() {
    objectModel = new Model(FLOOR_MODEL);
    objectModel->Translate(glm::vec3(0.0f, -1.1f, 0.0f));

    objectShader = new PhiShader(vertexShaderPath, fragShaderPath);
}

inline Floor::~Floor() {
    delete objectModel;
    delete objectShader;
}

inline void Floor::Render(Light *light, const glm::mat4 &view, const glm::mat4 &proj) {
    objectShader->Use();
    // TODO: Set Light
    light->SetShader(objectShader);
    // TODO: Set Eye position
    objectShader->SetUniform("eye_position", glm::vec3(0.0f, 0.0f, 3.0f));
    
    objectShader->SetUniform("model", objectModel->GetModelMatrix());
    objectShader->SetUniform("view", view);
    objectShader->SetUniform("projection", proj);
    
    objectModel->Draw(*objectShader);
}

#endif // FLOOR_H__