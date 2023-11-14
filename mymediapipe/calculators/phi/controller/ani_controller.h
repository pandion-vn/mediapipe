#ifndef ANIMATIONCONTROLLER_H__
#define ANIMATIONCONTROLLER_H__

#include "../common.h"
#include "abstract_controller.h"
#include "../phi_shader.h"
#include "../model.h"
#include "../rendering/light.h"

class AnimationController : public AbstractController {
public:
    AnimationController();
    ~AnimationController();
    void Init(/*int argc, char* argv[]*/) override;
    void Draw() override;
};

inline AnimationController::AnimationController()
    : AbstractController("The Revenge of Darth Maul") {
}

inline AnimationController::~AnimationController() {
    // delete d_animations;
    // delete d_bone_location;
}

inline void AnimationController::Init(int argc, char* argv[]) {
}

inline void AnimationController::Draw() {
}

#endif