#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "gl_base_calculator.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "lib/stb_image.h"
#include "lib/shader.h"
#include "lib/camera.h"
#include "lib/model.h"
#include "lib/video_scene.h"
#include "lib/model_scene.h"
#include "lib/animation.h"
#include "lib/animator.h"
// #include "lib/util.h"

namespace mediapipe {

enum { ATTRIB_VERTEX, ATTRIB_NORMAL, ATTRIB_TEXTURE_COORDS, LIGHTING_NUM_ATTRIBUTES };

// https://learnopengl.com/Model-Loading/Mesh
// https://learnopengl.com/Model-Loading/Model
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
class Gl18CameraSkeletalAnimationCalculator : public GlBaseCalculator {
public:
    absl::Status GlSetup() override;
    absl::Status GlBind() override;
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) override;
    absl::Status GlCleanup() override;
    absl::Status GlTeardown() override;
private:
    // GLuint VBO[3], cubeVAO, lightCubeVAO, EBO, TextureBO[2];
    // GLuint diffuseMapTexture;
    // GLuint VBO[2], VAO;
    VideoScene *videoScene;
    ModelScene *modelScene;
    int counting;
};

REGISTER_CALCULATOR(Gl18CameraSkeletalAnimationCalculator);

absl::Status Gl18CameraSkeletalAnimationCalculator::GlSetup() {

    // diffuseMapTexture = loadTexture("mymediapipe/assets/opengl/cube2/Square swirls.png");
    videoScene = new VideoScene();
    videoScene->Setup();

    modelScene = new ModelScene();
    modelScene->Setup();

    counting = 0;

    LOG(INFO) << "DONE setup";
    return absl::OkStatus();
}

absl::Status Gl18CameraSkeletalAnimationCalculator::GlBind() {    
    glEnable(GL_BLEND);
    // glEnable(GL_DEPTH_TEST);
    // glDepthMask(GL_TRUE);
    return absl::OkStatus();
}

absl::Status Gl18CameraSkeletalAnimationCalculator::GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) {
    // make sure we clear the framebuffer's content
    int src_width = dst.width();
    int src_height = dst.height();
    double deltaTime = timestamp - animation_start_time_;
    // std::cout << "FPS: " << (1.0f / deltaTime) << std::endl;
    animation_start_time_ = timestamp;

    // if (counting > 550) 
    //     counting = 0;
    // else if (counting > 300)
    //     animator->PlayAnimation(animation4);
    // else if (counting > 250)
    //     animator->PlayAnimation(animation3);
    // else if (counting > 200)
    //     animator->PlayAnimation(animation2);
    // else
    //     animator->PlayAnimation(animation1);
    
    // counting++;
    // animator->UpdateAnimation(deltaTime);

    // camera
    // Camera camera(glm::vec3(0.0f, -9.0f, 80.0f)); // aj
    // Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // vanguard

    videoScene->Draw(*framebuffer_target_, src.target(), src.name(), src_width, src_height);
    modelScene->Draw(*framebuffer_target_, src_width, src_height, deltaTime);

    // ourShader->use();

    // view/projection transformations
    // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
    //                                         (float)src_width / (float)src_height, 
    //                                         0.1f,
    //                                         100.0f);
    // glm::mat4 view = camera.GetViewMatrix();
    // ourShader->setMat4("projection", projection);
    // ourShader->setMat4("view", view);

    // auto transforms = animator->GetFinalBoneMatrices();
    // // std::cout << "Transform size: " << transforms.size() << std::endl;
    // for (int i = 0; i < transforms.size(); ++i) {
    //     ourShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    //     // std::cout << "Transform " << i << ": " << glm::to_string(transforms[i]) << std::endl;
    // }
    // ourShader->setMat4("finalBonesMatrices", transforms);

    // render the loaded model
    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(0.0f, 3.5f, 0.0f)); // translate it down so it's at the center of the scene
    // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    // model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
    // model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));	// it's a bit too big for our scene, so scale it down
    // model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // model = glm::rotate(model, (float) timestamp * glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 1.0f));  
    // ourShader->setMat4("model", model);

    // glEnable(GL_DEPTH_TEST);
    // glDepthMask(GL_TRUE);
    // framebuffer_target_->Bind();
    // // bind diffuse map
    // // glActiveTexture(GL_TEXTURE0);
    // // glBindTexture(GL_TEXTURE_2D, diffuseMapTexture);
    // // ourShader->setInt("texture_diffuse1", 0);
    // ourShader->use();
    // ourModel->Draw(*ourShader);

    // framebuffer_target_->Unbind();
    // glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_FALSE);

    return absl::OkStatus();    
}

absl::Status Gl18CameraSkeletalAnimationCalculator::GlCleanup() {
    // cleanup
    // glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_FALSE);
    glDisable(GL_BLEND);
    glFlush();
    return absl::OkStatus();    
}

absl::Status Gl18CameraSkeletalAnimationCalculator::GlTeardown() {
    // ourShader->tearDown();
    return absl::OkStatus();
}

} // mediapipe