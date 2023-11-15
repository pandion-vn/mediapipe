#ifndef ANIMATIONCONTROLLER_H__
#define ANIMATIONCONTROLLER_H__

#include "../common.h"
#include "abstract_controller.h"
#include "../phi_shader.h"
#include "../model.h"
#include "../rendering/light.h"
#include "../animation/ik_animator.h"

class AnimationController : public AbstractController {
private:
    PhiShader*      d_shader;
    PhiShader*      d_shader_bones;
    // PhiShader*   d_shader_bones_noTexture;
    // PhiShader*   d_shader_noTexture;
    Model*          d_model_dartmaul;
    IAnimation*     dartMaulAnimator;
    glm::vec3       d_light_position;

public:
    AnimationController();
    ~AnimationController();
    void Init(/*int argc, char* argv[]*/) override;
    void Draw() override;
};

inline AnimationController::AnimationController()
    : AbstractController() {
    d_light_ambient = glm::vec3(0.2f, 0.2f, 0.2f); // 0.2
    d_light_diffuse = glm::vec3(0.5f, 0.5f, 0.5f); // 0.5
    d_light_specular = glm::vec3(0.5f, 0.5f, 0.5f); // 0.5
}

inline AnimationController::~AnimationController() {
    // delete d_animations;
    // delete d_bone_location;
    delete d_shader;
    delete d_shader_bones;
}

inline void AnimationController::Init(/*int argc, char* argv[]*/) {
    AbstractController::Init();

    std::string v_shader = "mymediapipe/assets/phi/shaders/fem.vs";
    std::string f_shader_texture = "mymediapipe/assets/phi/shaders/fem.fs";
    std::string v_shaderBone = "mymediapipe/assets/phi/shaders/ani_bone.vs";
    std::string f_shader_bone_texture = "mymediapipe/assets/phi/shaders/ani_bone.fs";
    d_shader = new PhiShader(v_shader, f_shader_texture);
    d_shader_bones = new PhiShader(v_shaderBone, f_shader_bone_texture);

    d_model_dartmaul = new Model(DART_MAUL_MODEL);
    // d_model_dartmaul->Scale(glm::vec3(1.2, 1.2, 1.2));
    // d_model_dartmaul->Rotate(glm::vec3(1,0,0), glm::radians(-90.0f));
    dartMaulAnimator = new IKAnimator(d_model_dartmaul->m_skeleton);
    // d_model_dartmaul->Scale(glm::vec3(3, 3, 3));
    // d_model_dartmaul->Translate(glm::vec3(0, -2, 0));

    d_camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    // d_camera->SetTarget(d_model_dartmaul->GetPositionVec() + glm::vec3(0, 5, 0));
    d_light_position = glm::vec3(-10.0f,20.0f,0.0f); 
}

inline void AnimationController::Draw() {
    d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 100.0f);
    Light light(d_light_position, d_light_ambient, d_light_diffuse, d_light_specular);
    // d_camera->SetTarget(d_model_dartmaul->GetPositionVec() + glm::vec3(0, 5, 0));
    
    d_view_matrix = d_camera->GetViewMatrix();
    // d_shader->Use();
    
    auto vpMatrix = d_projection_matrix * d_view_matrix;

    d_shader_bones->Use();
    light.SetShader(d_shader_bones);
    glm::vec3 target = glm::vec3(0.9, -0.3, -1.9);
    glm::vec3 hand_l = d_model_dartmaul->m_skeleton->GetBonePosition("hand_L", d_model_dartmaul->GetModelMatrix());
    hand_l.x = hand_l.x - 0.3; 
    std::cout << "left hand: " << glm::to_string(hand_l) << std::endl;
    // glm::vec3 hand_r = d_model_dartmaul->m_skeleton->GetBonePosition("hand_R", d_model_dartmaul->GetModelMatrix());
    // std::cout << "right hand: " << glm::to_string(hand_r) << std::endl;
    // std::cout << "model matrix: " << glm::to_string(d_model_dartmaul->GetModelMatrix()) << std::endl;
    // std::vector<glm::vec3> positions = d_model_dartmaul->m_skeleton->GetBonePositions(d_model_dartmaul->GetModelMatrix());
    // for (int i=0; i<positions.size(); i++) {
    //     std::cout << "position[" << i << "] " << glm::to_string(positions[i]) << std::endl;
    // }

    d_model_dartmaul->Animate(dartMaulAnimator, hand_l, "hand_L");
    // d_shader_bones->SetUniform("model", vpMatrix * d_model_dartmaul->GetModelMatrix()); 
    // d_shader_bones->SetUniform("eye_position", d_camera->Position);  
    // d_shader_bones->SetUniform("mv",   d_view_matrix * d_model_dartmaul->GetModelMatrix());
    d_shader_bones->SetUniform("projection", d_projection_matrix);
    d_shader_bones->SetUniform("view", d_view_matrix);
    d_shader_bones->SetUniform("model", d_model_dartmaul->GetModelMatrix());
    // d_shader_bones->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_model_dartmaul->GetModelMatrix())));  
    // d_shader_bones->SetUniform("light_position", d_light_position);  

    d_model_dartmaul->Draw(*d_shader_bones);
}

#endif