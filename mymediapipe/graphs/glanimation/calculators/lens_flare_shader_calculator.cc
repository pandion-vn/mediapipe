#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "gl_base_calculator.h"
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
class LensFlareShaderCalculator : public GlBaseCalculator {
public:    
    absl::Status GlSetup() override;
    absl::Status GlBind() override;
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) override;
    absl::Status GlTeardown() override;

private:
    GLuint program_ = 0;
    GLint frame_;
    GLint projection_;
    GLint model_;
    GLint texture_;
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
        uniform mat4 projection_mat;
        uniform mat4 model_mat;

        attribute vec4 position;
        attribute vec4 tex_coord;

        varying vec2 v_tex_coord;

        void main() {
            v_tex_coord = tex_coord.xy;
            gl_Position = projection_mat * model_mat * position;
        }
    )";
    const GLchar* frag_src = R"(
        precision mediump float;

        varying vec2 v_tex_coord;
        uniform sampler2D texture;

        void main() {
            gl_FragColor = texture2D(texture, v_tex_coord);
        }
    )";

    // shader program
    GlhCreateProgram(vert_src, frag_src, NUM_ATTRIBUTES,
                    (const GLchar**)&attr_name[0], attr_location, &program_);
    RET_CHECK(program_) << "Problem initializing the program.";

    model_ = glGetUniformLocation(program_, "model_mat");
    projection_ = glGetUniformLocation(program_, "projection_mat");
    texture_ = glGetUniformLocation(program_, "texture");

    RET_CHECK_NE(projection_, -1)
        << "Failed to find `projection_mat` uniform!";
    RET_CHECK_NE(model_, -1)
        << "Failed to find `model_mat` uniform!";
    RET_CHECK_NE(texture_, -1) << "Failed to find `texture` uniform!";
    
    return absl::OkStatus();
}

absl::Status LensFlareShaderCalculator::GlBind() {
    return absl::OkStatus();
}

absl::Status LensFlareShaderCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                                 double timestamp) {

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