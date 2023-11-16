#ifndef FEMCONTROLLER_H__
#define FEMCONTROLLER_H__

#include "../common.h"
#include "abstract_controller.h"
#include "../phi_shader.h"
#include "../model.h"
#include "../rendering/light.h"

class FemController: public AbstractController {
public:
    FemController();
    ~FemController();
    void Init(/*int argc, char* argv[]*/);
    void Draw(double timestamp);

private:
    PhiShader*          d_shader;
    Model*              d_floor;
    Model*              d_model;

};

FemController::FemController()
    : AbstractController()  {
    // setup_current_instance();
}

FemController::~FemController() {
    delete d_shader;
    delete d_floor;
    delete d_model;
}

void FemController::Init(/*int argc, char* argv[]*/) {
    AbstractController::Init(/*argc, argv*/);
    d_floor = new Model(FLOOR_MODEL);
    d_floor->Translate(glm::vec3(0.0f, -0.4f, 0.0f));

    d_camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));

    d_model = new Model(DART_MAUL_MODEL);
    // d_model->Scale(glm::vec3(3, 3, 3));
    // d_model->Translate(glm::vec3(0, -2, 0));
    // d_model->Scale(glm::vec3(0.2f, 0.2f, 0.2f));
    d_model->Rotate(glm::vec3(1,0,0), glm::radians(-90.0f));

    std::string v_shader = "mymediapipe/assets/phi/shaders/fem.vs";
    std::string f_shader = "mymediapipe/assets/phi/shaders/fem.fs";
    d_shader = new PhiShader(v_shader, f_shader);
    // d_shader->SetUniform("roughnessValue", 0.8f);
    // d_shader->SetUniform("fresnelReflectance", 0.8f);
    d_shader->SetUniform("eye_position", d_camera->Position);
    // d_shader->SetUniform("use_bump_mapping", false);
}

void FemController::Draw(double timestamp) {
    d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 100.0f);
    d_view_matrix = d_camera->GetViewMatrix();
    // std::cout << "d_view_matrix: " << glm::to_string(d_view_matrix) << std::endl;
    Light light(d_light_position, d_light_ambient, d_light_diffuse, d_light_specular);
    
    d_shader->Use();
    light.SetShader(d_shader);
    d_shader->SetUniform("view", d_view_matrix);
    d_shader->SetUniform("projection", d_projection_matrix);
    d_shader->SetUniform("model", d_model->GetModelMatrix());
    // d_shader->SetUniform("mv", d_view_matrix * d_floor->GetModelMatrix());
    // d_shader->SetUniform("model_matrix", d_floor->GetModelMatrix());
    // d_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_floor->GetModelMatrix())));
    d_model->Draw(*d_shader);

    d_shader->Use();
    d_shader->SetUniform("view", d_view_matrix);
    d_shader->SetUniform("projection", d_projection_matrix);
    d_shader->SetUniform("model", d_floor->GetModelMatrix());

    d_floor->Draw(*d_shader);
}

#endif 