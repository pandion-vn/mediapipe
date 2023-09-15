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
class TriangleShaderCalculator : public GlSimpleCalculator {
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

REGISTER_CALCULATOR(TriangleShaderCalculator);

absl::Status TriangleShaderCalculator::GlSetup() {
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
        // uniform sampler2D texture;

        void main() {
            // gl_FragColor = texture2D(texture, v_tex_coord);
            gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
        }
    )";

    // shader program
    GlhCreateProgram(vert_src, frag_src, NUM_ATTRIBUTES,
                    (const GLchar**)&attr_name[0], attr_location, &program_);
    RET_CHECK(program_) << "Problem initializing the program.";

    // GLint frame_ = glGetUniformLocation(program_, "texture");
    
    return absl::OkStatus();
}

absl::Status TriangleShaderCalculator::GlRender(const GlTexture& src, const GlTexture& dst) {
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

    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
    glEnableVertexAttribArray ( 0 );
    glDrawArrays ( GL_TRIANGLES, 0, 3 );

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glUseProgram(0);
    glFlush();
    
    return absl::OkStatus();
}

absl::Status TriangleShaderCalculator::GlTeardown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    return absl::OkStatus();
}

}  // namespace mediapipe