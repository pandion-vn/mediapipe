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
        #version 310 es
        precision mediump float;

        in vec3 iPosition;
        in vec3 iNormal;
        in vec2 iTexCoords;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec2 TexCoords;

        void main()
        {
            TexCoords = iTexCoords;
            
            gl_Position = projection * view * model * vec4(iPosition, 1.0);
        }
    )";

    const GLchar* frag_src = 
    R"(
        #version 310 es
        precision mediump float;

        out vec4 fragColor;

        in vec2 TexCoords;

        uniform sampler2D texture_diffuse1;

        void main()
        {
            fragColor = texture2D(texture_diffuse1, TexCoords);
            // fragColor = vec4(1.0);
        } 
    )";

    const GLchar* cube_vert_src = R"(
        #version 310 es
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
        precision mediump float;
        out vec4 fragColor;

        void main()
        {
            fragColor = vec4(1.0);
        } 
    )";

    ourShader = new Shader(vert_src, frag_src);

    // load models
    ourModel = new Model("mymediapipe/assets/opengl/minecraft_block/Grass_Block.obj");

    // stbi_set_flip_vertically_on_load(true);
    // diffuseMapTexture = loadTexture("mymediapipe/assets/opengl/viking/viking_room.png");

    std::cout << "DONE setup" << std::endl;
    return absl::OkStatus();
}

absl::Status Gl16ModelLoadingCalculator::GlBind() {

    // ourShader->use();
    
    return absl::OkStatus();
}

absl::Status Gl16ModelLoadingCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                                      double timestamp) {

    // Set up the GL state.
    glEnable(GL_DEPTH_TEST);
    // glDepthMask(GL_TRUE);

    // glClearColor(0.f, 0.f, 0.f, 0.f);
    // glClearDepthf(1.f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int src_width = src.width();
    int src_height = src.height();
    // glViewport(0, 0, src_width, src_height);

    // camera
    Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

    // ourShader->use();
    // ourShader->setInt("texture_diffuse1", 0);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)src_width / (float)src_height, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    // model = glm::rotate(model, glm::radians(70.0f), glm::vec3(1.0f, 0.0f, 1.0f));
    model = glm::rotate(model, (float) timestamp * glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 1.0f));  
    ourShader->setMat4("model", model);

    // bind diffuse map
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, diffuseMapTexture);

    ourModel->Draw(*ourShader);

    return absl::OkStatus();    
}

absl::Status Gl16ModelLoadingCalculator::GlCleanup() {
    // Restore the GL state.
    // cleanup
    // Restore the GL state.
    // glDepthMask(GL_FALSE);
    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_BLEND);

    // glUseProgram(0);
    // glFlush();
    return absl::OkStatus();    
}

absl::Status Gl16ModelLoadingCalculator::GlTeardown() {
    ourShader->tearDown();
    return absl::OkStatus();
}

} // mediapipe