#ifndef FEMCONTROLLER_H__
#define FEMCONTROLLER_H__

#include "../common.h"
#include "abstract_controller.h"
#include "../phi_shader.h"
#include "../model.h"

class FemController: public AbstractController {
public:
    FemController();
    ~FemController();
    void Init(/*int argc, char* argv[]*/);
    void Draw();

private:
    PhiShader*         d_shader;
    Model*          d_floor;

};

FemController::FemController()
    : AbstractController()  {
    // setup_current_instance();
}

FemController::~FemController() {
    delete d_shader;
    delete d_floor;
}

void FemController::Init(/*int argc, char* argv[]*/) {
    AbstractController::Init(/*argc, argv*/);
    d_floor = new Model(FLOOR_MODEL);
    d_floor->Translate(glm::vec3(0.0f, -0.4f, 0.0f));

    d_camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    // d_camera->CameraType = FREE_FLY;
    // d_camera->MovementSpeed = 0.5f;
	// d_camera->SetTarget(glm::vec3(0.0, 0.0, 0.0));

    std::string v_shader = "mymediapipe/assets/phi/shaders/fem.vs";
    std::string f_shader = "mymediapipe/assets/phi/shaders/fem.fs";
    d_shader = new PhiShader(v_shader, f_shader);
    // d_shader->SetUniform("roughnessValue", 0.8f);
    // d_shader->SetUniform("fresnelReflectance", 0.8f);
    // d_shader->SetUniform("eye_position", d_camera->Position);  
    // d_shader->SetUniform("use_bump_mapping", false);
}

void FemController::Draw() {
    d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 100.0f);
    d_view_matrix = d_camera->GetViewMatrix();
    // std::cout << "d_view_matrix: " << glm::to_string(d_view_matrix) << std::endl;
    d_shader->Use();
    glm::vec3 position = glm::vec3(0, 0, 0) + 0.0001f;
    glm::vec3 scale = glm::vec3(.15f, .15f, .15f);
    float pitch = 10.0f;
    float yaw = 0.0f;
    glm::mat4 model;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1, 0, 0));
    R = glm::rotate(R, yaw, glm::vec3(0, 0, 1));
    model = T * R * S;
    d_shader->SetUniform("view", d_view_matrix);
    d_shader->SetUniform("projection", d_projection_matrix);
    d_shader->SetUniform("model", model);
    // d_shader->SetUniform("mv", d_view_matrix * d_floor->GetModelMatrix());
    // d_shader->SetUniform("model_matrix", d_floor->GetModelMatrix());
    // d_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_floor->GetModelMatrix())));
    d_floor->Draw(*d_shader);
}

#endif 