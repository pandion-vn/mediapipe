#ifndef ANIMATIONCONTROLLER_H__
#define ANIMATIONCONTROLLER_H__

#include "../common.h"
#include "abstract_controller.h"
#include "../phi_shader.h"
#include "../model.h"
#include "../rendering/light.h"
#include "../animation/ik_animator.h"
#include "../target.h"
#include "../pose.h"

class AnimationController : public AbstractController {
private:
    PhiShader*      d_shader;
    PhiShader*      d_shader_bones;
    // PhiShader*   d_shader_bones_noTexture;
    // PhiShader*   d_shader_noTexture;
    Model*          d_model_dartmaul;
    IAnimation*     dartMaulAnimator;
    glm::vec3       d_light_position;
    glm::vec3       target_position;
    Target*         target;
    std::vector<glm::vec3> lm3d;
    Pose*           pose;

public:
    AnimationController();
    ~AnimationController();
    void Init(/*int argc, char* argv[]*/) override;
    void Draw(double timestamp) override;
    void SetLandmarks(std::vector<glm::vec3> &lm3d);
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

inline void AnimationController::SetLandmarks(std::vector<glm::vec3> &lm3d) {
    this->lm3d = lm3d;
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
    d_model_dartmaul->Translate(glm::vec3(0, -0.8, 0));

    d_camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    d_light_position = glm::vec3(-10.0f, 20.0f, 0.0f); 

    target_position = glm::vec3(-0.7f, 1.8f, 0.7f);
    target = new Target(target_position.x, target_position.y, target_position.z);

    AngleRestriction head_restrict(-30.0f, 30.0f, -20.0f, 20.0f, -10.0f, 10.0f);
    AngleRestriction arms_restrict(-30.0f, 30.0f, -60.0f, 60.0f, -30.0f, 30.0f);
    AngleRestriction fingers_restrict(0.0f, 0.0f, -10.0f, 10.0f, -10.0f, 10.0f);
    AngleRestriction legs_restrict(-30.0f, 30.0f, -60.0f, 60.0f, -30.0f, 30.0f);

    d_model_dartmaul->SetJointLimit("neck", head_restrict);
    d_model_dartmaul->SetJointLimit("head", head_restrict);
    d_model_dartmaul->SetJointLimit("shoulder_L", arms_restrict);
    d_model_dartmaul->SetJointLimit("upper_arm_L", arms_restrict);
    d_model_dartmaul->SetJointLimit("forearm_L", arms_restrict);
    d_model_dartmaul->SetJointLimit("hand_L", arms_restrict);

    d_model_dartmaul->SetJointLimit("shoulder_R", arms_restrict);
    d_model_dartmaul->SetJointLimit("upper_arm_R", arms_restrict);
    d_model_dartmaul->SetJointLimit("forearm_R", arms_restrict);
    d_model_dartmaul->SetJointLimit("hand_R", arms_restrict);

    d_model_dartmaul->SetJointLimit("thigh_L", legs_restrict);
    d_model_dartmaul->SetJointLimit("thigh_R", legs_restrict);
    d_model_dartmaul->SetJointLimit("shin_L", legs_restrict);
    d_model_dartmaul->SetJointLimit("shin_R", legs_restrict);
    d_model_dartmaul->SetJointLimit("foot_L", legs_restrict);
    d_model_dartmaul->SetJointLimit("foot_R", legs_restrict);
    // d_model_dartmaul->SetJointLimit("thumb.L",fingers);
    // d_model_dartmaul->SetJointLimit("fingers.L",fingers);
    // d_model_dartmaul->SetJointLimit("fingerstip.L",fingers);
    pose = new Pose();
}

inline void AnimationController::Draw(double timestamp) {
    Light light(d_light_position, d_light_ambient, d_light_diffuse, d_light_specular);
    d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 100.0f);
    d_view_matrix = d_camera->GetViewMatrix();
    auto vpMatrix = d_projection_matrix * d_view_matrix;
    glm::vec3 new_pos = target_position;
    new_pos.y = target_position.y * sin(timestamp)/2.0 + 1.0;

    d_shader_bones->Use();
    light.SetShader(d_shader_bones);
    
    // glm::vec3 hand_l = d_model_dartmaul->m_skeleton->GetBonePosition("hand_L", d_model_dartmaul->GetModelMatrix());
    // hand_l.x = hand_l.x - 0.3; 
    // std::cout << "left hand: " << glm::to_string(hand_l) << std::endl;
    // glm::vec3 hand_r = d_model_dartmaul->m_skeleton->GetBonePosition("hand_R", d_model_dartmaul->GetModelMatrix());
    // std::cout << "right hand: " << glm::to_string(hand_r) << std::endl;
    // std::cout << "model matrix: " << glm::to_string(d_model_dartmaul->GetModelMatrix()) << std::endl;
    // std::vector<glm::vec3> positions = d_model_dartmaul->m_skeleton->GetBonePositions(d_model_dartmaul->GetModelMatrix());
    // for (int i=0; i<positions.size(); i++) {
    //     std::cout << "position[" << i << "] " << glm::to_string(positions[i]) << std::endl;
    // }

    // d_model_dartmaul->Animate(dartMaulAnimator, lm3d[0], "head", 2);
    d_model_dartmaul->Animate(dartMaulAnimator, lm3d[15], "hand_R", 3);
    d_model_dartmaul->Animate(dartMaulAnimator, lm3d[16], "hand_L", 3);
    d_model_dartmaul->Animate(dartMaulAnimator, lm3d[27], "foot_R", 2);
    d_model_dartmaul->Animate(dartMaulAnimator, lm3d[28], "foot_L", 2);

    // d_shader_bones->SetUniform("model", vpMatrix * d_model_dartmaul->GetModelMatrix()); 
    // d_shader_bones->SetUniform("eye_position", d_camera->Position);  
    // d_shader_bones->SetUniform("mv",   d_view_matrix * d_model_dartmaul->GetModelMatrix());
    d_shader_bones->SetUniform("projection", d_projection_matrix);
    d_shader_bones->SetUniform("view", d_view_matrix);
    d_shader_bones->SetUniform("model", d_model_dartmaul->GetModelMatrix());
    // d_shader_bones->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_model_dartmaul->GetModelMatrix())));  
    // d_shader_bones->SetUniform("light_position", d_light_position);  

    d_model_dartmaul->Draw(*d_shader_bones);

    target->SetPosition(new_pos);
    target->Render(d_view_matrix, d_projection_matrix);

    pose->SetLandmarks(lm3d);
    pose->Render(d_view_matrix, d_projection_matrix);
}

#endif