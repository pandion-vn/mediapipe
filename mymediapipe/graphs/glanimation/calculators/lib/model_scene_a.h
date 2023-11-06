#ifndef MODEL_SCENE_A_H
#define MODEL_SCENE_A_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "shader.h"
#include "animation.h"
#include "animator.h"
#include "bone.h"

class ModelSceneA {
public:
    ModelSceneA():counting(0) {}
    ~ModelSceneA() {}

    void Setup() {
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
                gl_Position = projection * view * model * updatedPosition;
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
                // fragColor = vec4(1.0);
            } 
        )";

        ourShader = new Shader(vert_src, frag_src);

        // load models
        // ourModel = new Model("mymediapipe/assets/opengl/aj/aj.dae");
        ourModel = new Model("mymediapipe/assets/opengl/michelle/michelle.dae");
        
        // animation = new Animation("mymediapipe/assets/opengl/aj/aj.dae", ourModel);
        animation = new Animation("mymediapipe/assets/opengl/michelle/michelle.dae", ourModel);
        animation->ShowBones();
        
        animator = new Animator();
        animator->PlayAnimation(animation);
    }

    void Draw(const FrameBufferTarget &framebuffer_target_, 
              int src_width, int src_height,
              double deltaTime,
              std::vector<glm::vec3> &landmarks,
              std::vector<glm::quat> &rotations) {
        counting++;
        
        animator->UpdateAnimationWithBone(deltaTime, landmarks, rotations);

        ourShader->use();

        // x, y, z
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, -1.0f, 0.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(65.f), (float)src_width / (float)src_height, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(camPos, position, cameraUp);
        ourShader->setMat4("projection", projection);
        ourShader->setMat4("view", view);

        auto transforms = animator->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i) {
            ourShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        // x, y, z
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f)); // translate down michelle
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f)); // translate far michelle
        model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));	// michelle & aj
        ourShader->setMat4("model", model);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        framebuffer_target_.Bind();
        glViewport(0, 0, src_width, src_height);

        // draw model
        ourShader->use();
        ourModel->Draw(*ourShader);

        framebuffer_target_.Unbind();
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    }


private:
    Shader *ourShader;
    Model *ourModel;
    Animation *animation, *animation1, *animation2, *animation3, *animation4, *animation5;
    Animator *animator;
    int counting;
};

#endif