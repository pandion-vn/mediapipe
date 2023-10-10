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
#include "lib/animation.h"
#include "lib/animator.h"
// #include "lib/util.h"

namespace mediapipe {

enum { ATTRIB_VERTEX, ATTRIB_NORMAL, ATTRIB_TEXTURE_COORDS, LIGHTING_NUM_ATTRIBUTES };

// https://learnopengl.com/Model-Loading/Mesh
// https://learnopengl.com/Model-Loading/Model
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
class Gl17SkeletalAnimationCalculator : public GlBaseCalculator {
public:
    absl::Status GlSetup() override;
    absl::Status GlBind() override;
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) override;
    absl::Status GlCleanup() override;
    absl::Status GlTeardown() override;
private:
    GLuint VBO[3], cubeVAO, lightCubeVAO, EBO, TextureBO[2];
    Shader *ourShader;
    Model *ourModel;
    Animation *danceAnimation;
    Animator *animator;
    GLuint diffuseMapTexture;
};

REGISTER_CALCULATOR(Gl17SkeletalAnimationCalculator);

absl::Status Gl17SkeletalAnimationCalculator::GlSetup() {
    const GLchar* vert_src = 
    R"(
        #version 320 es
        precision highp float;
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 normal;
        layout (location = 2) in vec2 texcoords;
        layout (location = 3) in vec3 tangent;
        layout (location = 4) in vec3 bitangent;
        layout (location = 5) in ivec4 boneIds;
        layout (location = 6) in vec4 weights;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        const int MAX_BONES = 100;
        const int MAX_BONE_INFLUENCE = 4;
        uniform mat4 finalBonesMatrices[MAX_BONES];


        // out vec3 outFragPos;
        out vec4 outFragPos;
        out vec3 outNormal;
        out vec2 outTexCoords;

        void main()
        {
            vec4 updatedPosition = vec4(0.0f);
            vec3 updatedNormal = vec3(0.0f);
            for (int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
                // Current bone-weight pair is non-existing
                if (boneIds[i] == -1) 
                    continue;
                // Ignore all bones over count MAX_BONES
                if (boneIds[i] >= MAX_BONES) {
                    updatedPosition = vec4(position, 1.0f);
                    break;
                }
                // Set pos
                vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(position, 1.0f);
                updatedPosition += localPosition * weights[i];
                // Set normal
                vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * normal;
                updatedNormal += localNormal * weights[i];
            }
            gl_Position = (projection * view * model) * updatedPosition;
            outFragPos = vec4(model * vec4(vec3(updatedPosition), 1.0));
            outNormal = updatedNormal;
            // outNormal = mat3(transpose(inverse(model))) * normal;
            outTexCoords = texcoords;
        }
    )";

    const GLchar* frag_src = 
    R"(
        #version 320 es
        precision highp float;

        out vec4 fragColor;

        in vec4 outFragPos;
        in vec3 outNormal;
        in vec2 outTexCoords;

        uniform sampler2D texture_diffuse1;
        // uniform sampler2D texture;

        void main()
        {
            // ambient 
            // vec3 light_ambient = vec3(0.2, 0.2, 0.2);
            // vec3 material_ambient = vec3(1.0, 0.5, 0.31);
            // vec3 ambient = light_ambient * material_ambient;
            // vec3 light_position = vec3(1.2, 1.0, 2.0);
            // vec3 light_diffuse = vec3(0.5, 0.5, 0.5);
            
            // diffuse 
            // vec3 norm = normalize(outNormal);
            // vec3 lightDir = normalize(light_position - outFragPos);
            // float diff = max(dot(norm, lightDir), 0.0);
            // vec3 diffuse = light_diffuse * diff * texture(texture_diffuse1, outTexCoords).rgb;

            // vec3 result = ambient + diffuse;
            // fragColor = vec4(result, 1.0);
            fragColor = texture(texture_diffuse1, outTexCoords);
        } 
    )";

    const GLchar* cube_vert_src = R"(
        #version 310 es
        precision highp float;
        in vec3 position;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {            
            gl_Position = projection * view * model * vec4(position, 1.0);
        }
    )";

    const GLchar* cube_frag_src = R"(
        #version 310 es
        precision highp float;
        out vec4 fragColor;

        void main()
        {
            gl_FragColor = vec4(1.0);
        } 
    )";

    ourShader = new Shader(vert_src, frag_src);

    // load models
    // ourModel = new Model("mymediapipe/assets/opengl/backpack1/backpack.obj");
    // ourModel = new Model("mymediapipe/assets/opengl/box-wood/box.obj");
    // ourModel = new Model("mymediapipe/assets/opengl/cube2/cube2.obj");
    // ourModel = new Model("mymediapipe/assets/opengl/planet/planet.obj");
    // ourModel = new Model("mymediapipe/assets/opengl/rock/rock.obj");
    // ourModel = new Model("mymediapipe/assets/opengl/cyborg/cyborg.obj");
    // ourModel = new Model("mymediapipe/assets/obj/IronMan.obj");

    stbi_set_flip_vertically_on_load(true);
    ourModel = new Model("mymediapipe/assets/opengl/vampire/dancing_vampire.dae");
    danceAnimation = new Animation("mymediapipe/assets/opengl/vampire/dancing_vampire.dae", ourModel);
	animator = new Animator();
    // diffuseMapTexture = loadTexture("mymediapipe/assets/opengl/cube2/Square swirls.png");

    LOG(INFO) << "DONE setup";
    return absl::OkStatus();
}

absl::Status Gl17SkeletalAnimationCalculator::GlBind() {    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    return absl::OkStatus();
}

absl::Status Gl17SkeletalAnimationCalculator::GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) {
    // make sure we clear the framebuffer's content
    int src_width = dst.width();
    int src_height = dst.height();
    double deltaTime = timestamp - animation_start_time_;
    std::cout << "FPS: " << (1.0f / deltaTime) << std::endl;
    animation_start_time_ = timestamp;

    animator->PlayAnimation(danceAnimation);
    animator->UpdateAnimation(deltaTime);

    // camera
    Camera camera(glm::vec3(0.0f, -9.0f, 80.0f));
    ourShader->use();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                                            (float)src_width / (float)src_height, 
                                            0.1f,
                                            100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);

    auto transforms = animator->GetFinalBoneMatrices();
    // std::cout << "Transform size: " << transforms.size() << std::endl;
    for (int i = 0; i < transforms.size(); ++i) {
        ourShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        // std::cout << "Transform " << i << ": " << glm::to_string(transforms[i]) << std::endl;
    }
    // ourShader->setMat4("finalBonesMatrices", transforms);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 3.5f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
    // model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // model = glm::rotate(model, (float) timestamp * glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 1.0f));  
    ourShader->setMat4("model", model);

    framebuffer_target_->Bind();
    // bind diffuse map
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, diffuseMapTexture);
    // ourShader->setInt("texture_diffuse1", 0);
    ourModel->Draw(*ourShader);
    framebuffer_target_->Unbind();

    return absl::OkStatus();    
}

absl::Status Gl17SkeletalAnimationCalculator::GlCleanup() {
    // cleanup
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glFlush();
    return absl::OkStatus();    
}

absl::Status Gl17SkeletalAnimationCalculator::GlTeardown() {
    return absl::OkStatus();
}

} // mediapipe