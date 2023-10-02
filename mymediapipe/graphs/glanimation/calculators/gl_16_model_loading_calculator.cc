#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "gl_base_calculator.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "lib/stb_image.h"
#include "lib/shader.h"
#include "lib/camera.h"
#include "lib/model.h"
// #include "lib/util.h"

namespace mediapipe {

enum { ATTRIB_VERTEX, ATTRIB_NORMAL, ATTRIB_TEXTURE_COORDS, LIGHTING_NUM_ATTRIBUTES };

// https://learnopengl.com/Model-Loading/Mesh
// https://learnopengl.com/Model-Loading/Model
class Gl16ModelLoadingCalculator : public GlBaseCalculator {
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
    GLuint diffuseMapTexture;
};

REGISTER_CALCULATOR(Gl16ModelLoadingCalculator);

absl::Status Gl16ModelLoadingCalculator::GlSetup() {
    const GLchar* vert_src = 
    R"(
        #version 320 es
        precision highp float;
        in vec3 position;
        in vec3 normal;
        in vec2 texcoords;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        // out vec3 outFragPos;
        out vec2 outTexCoords;

        void main()
        {
            // outFragPos = vec3(model * vec4(position, 1.0));
            outTexCoords = texcoords;
            // gl_Position = projection * view * vec4(outFragPos, 1.0);
            gl_Position = projection * view * model * vec4(position, 1.0);
        }
    )";

    const GLchar* frag_src = 
    R"(
        #version 320 es
        precision highp float;

        out vec4 fragColor;

        // in vec3 outFragPos;
        in vec2 outTexCoords;

        uniform sampler2D texture_diffuse1;
        // uniform sampler2D texture;

        void main()
        {
            fragColor = texture(texture_diffuse1, outTexCoords);
            // fragColor = vec4(1.0);
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
    ourModel = new Model("mymediapipe/assets/opengl/planet/planet.obj");

    // stbi_set_flip_vertically_on_load(true);
    // diffuseMapTexture = loadTexture("mymediapipe/assets/opengl/box-wood/Box-wood_TEX.png");

    std::cout << "DONE setup" << std::endl;
    return absl::OkStatus();
}

absl::Status Gl16ModelLoadingCalculator::GlBind() {    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    return absl::OkStatus();
}

absl::Status Gl16ModelLoadingCalculator::GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) {
    // make sure we clear the framebuffer's content
    int src_width = src.width();
    int src_height = src.height();

    // camera
    Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
    ourShader->use();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)src_width / (float)src_height, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    // model = glm::rotate(model, glm::radians(70.0f), glm::vec3(1.0f, 0.0f, 1.0f));
    model = glm::rotate(model, (float) timestamp * glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 1.0f));  
    ourShader->setMat4("model", model);

    ourModel->Draw(*ourShader, *framebuffer_target_);

    return absl::OkStatus();    
}

absl::Status Gl16ModelLoadingCalculator::GlCleanup() {
    // cleanup
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(0);
    glFlush();
    return absl::OkStatus();    
}

absl::Status Gl16ModelLoadingCalculator::GlTeardown() {
    // ourShader->tearDown();
    return absl::OkStatus();
}

} // mediapipe