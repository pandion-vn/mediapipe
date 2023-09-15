#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_calculator.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum { ATTRIB_VERTEX, ATTRIB_TEXTURE_POSITION, NUM_ATTRIBUTES };

namespace mediapipe {

// Lens Flare Example 
// https://www.shadertoy.com/view/4sX3Rs
// This is free and unencumbered software released into the public domain. https://unlicense.org/
// Trying to get some interesting looking lens flares, seems like it worked. 
// See https://www.shadertoy.com/view/lsBGDK for a more avanced, generalized solution
// If you find this useful send me an email at peterekepeter at gmail dot com, 
// I've seen this shader pop up before in other works, but I'm curious where it ends up.
// If you want to use it, feel free to do so, there is no need to credit but it is appreciated.
class LensFlareShaderCalculator : public GlSimpleCalculator {
public:
    absl::Status GlSetup() override;
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst) override;
    absl::Status GlTeardown() override;

private:
    GLuint program_ = 0;
    GLint frame_;
    GLint projection_;
    GLint model_;
};

REGISTER_CALCULATOR(LensFlareShaderCalculator);

absl::Status LensFlareShaderCalculator::GlSetup() {
    // Load vertex and fragment shaders
    const GLint attr_location[NUM_ATTRIBUTES] = {
        ATTRIB_VERTEX,
        ATTRIB_TEXTURE_POSITION,
    };

    const GLchar* attr_name[NUM_ATTRIBUTES] = {
        "position",
        "tex_coord",
    };

    const GLchar* vert_src = R"(
        attribute vec4 position;
        attribute vec4 tex_coord;
        varying vec2 v_tex_coord;

        void main() {
            v_tex_coord = tex_coord.xy;
            gl_Position = position;
        }
    )";
    const GLchar* frag_src = R"(
        precision mediump float;

        varying vec2 v_tex_coord;
        uniform sampler2D texture;

        void main() {
            // gl_FragColor = texture2D(texture, v_tex_coord);
            gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
        }
    )";

    // shader program
    GlhCreateProgram(vert_src, frag_src, NUM_ATTRIBUTES,
                    (const GLchar**)&attr_name[0], attr_location, &program_);
    RET_CHECK(program_) << "Problem initializing the program.";

    // model_ = glGetUniformLocation(program_, "model");
    // projection_ = glGetUniformLocation(program_, "projection");
    // frame_ = glGetUniformLocation(program_, "sprite");
    GLint frame_ = glGetUniformLocation(program_, "texture");
    
    return absl::OkStatus();
}

absl::Status LensFlareShaderCalculator::GlRender(const GlTexture& src, const GlTexture& dst) {
    // static const GLfloat square_vertices[] = {
    //     -1.0f, -1.0f,  // bottom left
    //     1.0f,  -1.0f,  // bottom right
    //     -1.0f, 1.0f,   // top left
    //     1.0f,  1.0f,   // top right
    // };
    // static const float texture_vertices[] = {
    //     0.0f, 0.0f,  // bottom left
    //     1.0f, 0.0f,  // bottom right
    //     0.0f, 1.0f,  // top left
    //     1.0f, 1.0f,  // top right
    // };
    // configure VAO/VBO
    // unsigned int VBO;
    static const float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    static const GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                                         -0.5f, -0.5f, 0.0f,
                                          0.5f, -0.5f, 0.0f };

    // program
    glUseProgram(program_);
    glEnable(GL_BLEND);
    glFrontFace(GL_CCW);

    // render OVERDRAW
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // glUniform1i(frame_, 1);

    // parameters
    // glUniform1i(glGetUniformLocation(this->ID, name), value);
    // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(src.width()), static_cast<float>(src.height()), 0.0f, -1.0f, 1.0f);
    // glUniformMatrix4fv(projection_, 1, false, glm::value_ptr(projection));
    // glUniform1i(frame_, 1);
    // glUniform1f(pixel_w_, 1.0 / src.width());
    // glUniform1f(pixel_h_, 1.0 / src.height());
    
    // glm::vec2 position = glm::vec2(0.0f, 0.0f);
    // glm::vec2 size = glm::vec2(src.width(), src.height());
    // float rotate = 0.0f;
    // glm::vec3 color = glm::vec3(1.0f);

    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)
    // model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    // model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    // model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back
    // model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    // glUniformMatrix4fv(model_, 1, false, glm::value_ptr(model));

    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
    glEnableVertexAttribArray ( 0 );
    glDrawArrays ( GL_TRIANGLES, 0, 3 );

    // vertex storage
    // GLuint vbo[2];
    // glGenBuffers(2, vbo);
    // GLuint vao;
    // glGenVertexArrays(1, &vao);
    // glBindVertexArray(vao);
    // unsigned int quadVAO;
    // unsigned int VBO;
    // glGenVertexArrays(1, &quadVAO);
    // glGenBuffers(1, &VBO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glBindVertexArray(quadVAO);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

    // vbo 0
    // glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    // glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), square_vertices,
    //             GL_STATIC_DRAW);
    // glEnableVertexAttribArray(ATTRIB_VERTEX);
    // glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, nullptr);

    // vbo 1
    // glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    // glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), texture_vertices,
    //             GL_STATIC_DRAW);
    // glEnableVertexAttribArray(ATTRIB_TEXTURE_POSITION);
    // glVertexAttribPointer(ATTRIB_TEXTURE_POSITION, 2, GL_FLOAT, 0, 0, nullptr);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, this->ID);

    // glBindVertexArray(this->quadVAO);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // glBindVertexArray(0);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glUseProgram(0);
    glFlush();

    // draw
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // cleanup
    // glDisableVertexAttribArray(ATTRIB_VERTEX);
    // glDisableVertexAttribArray(ATTRIB_TEXTURE_POSITION);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
    // glDeleteVertexArrays(1, &vao);
    // glDeleteBuffers(2, vbo);
    // glDeleteVertexArrays(1, &quadVAO);
    
    return absl::OkStatus();
}

absl::Status LensFlareShaderCalculator::GlTeardown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    return absl::OkStatus();
}

}  // namespace mediapipe