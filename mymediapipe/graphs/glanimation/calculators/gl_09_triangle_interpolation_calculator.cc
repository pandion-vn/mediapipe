#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "gl_base_calculator.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


namespace mediapipe {

enum { ATTRIB_VERTEX, ATTRIB_COLOR, NUM_ATTRIBUTES };

// https://ogldev.org/www/tutorial06/tutorial06.html
class Gl09TriangleInterpolationCalculator : public GlBaseCalculator {
public:
    absl::Status GlSetup() override;
    absl::Status GlBind() override;
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) override;
    absl::Status GlCleanup() override;
    absl::Status GlTeardown() override;
private:
    GLuint program_ = 0;
    GLint texture_;
    GLint scale_, transform_;
    GLuint VBO[2], VAO, EBO;
};

REGISTER_CALCULATOR(Gl09TriangleInterpolationCalculator);

absl::Status Gl09TriangleInterpolationCalculator::GlSetup() {

    // Load vertex and fragment shaders
    const GLint attr_location[NUM_ATTRIBUTES] = {
        ATTRIB_VERTEX,
        ATTRIB_COLOR,
    };

    const GLchar* attr_name[NUM_ATTRIBUTES] = {
        "position",
        "color",
    };

    const GLchar* vert_src = R"(
        attribute vec3 position;
        attribute vec3 color;
        // uniform float gScale;
        uniform mat4 transform;
        varying vec3 ourColor;

        void main()
        {
            gl_Position = transform * vec4(position, 1.0);
            ourColor = color;
        }
    )";

    const GLchar* frag_src = R"(
        precision mediump float;
        varying vec3 ourColor;

        void main()
        {
            // gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
            gl_FragColor = vec4(ourColor, 1.0);
        } 
    )";

    // shader program
    GlhCreateProgram(vert_src, frag_src, NUM_ATTRIBUTES,
                    (const GLchar**)&attr_name[0], attr_location, &program_);
    RET_CHECK(program_) << "Problem initializing the program.";
    // scale_ = glGetUniformLocation(program_, "gScale");
    // RET_CHECK_NE(scale_, -1) << "Failed to find `gScale` uniform!";
    transform_ = glGetUniformLocation(program_, "transform");
    RET_CHECK_NE(transform_, -1) << "Failed to find `transform` uniform!";

    return absl::OkStatus();
}

absl::Status Gl09TriangleInterpolationCalculator::GlBind() {

    glm::mat4 transform = glm::mat4(1.0);
    // transform = glm::translate(transform, glm::vec3(0.3, -0.3, 0.0));
    

    // Initial potition of dot
    GLfloat vertices[] = {
        // positions        
        -1.0f, -1.0f, 0.0f, 
         1.0f, -1.0f, 0.0f, 
         0.0f,  1.0f, 0.0f, 
    };
    GLfloat colors[] = {
        // colors
        1.0f, 0.0f, 0.0f, // bottom right
        0.0f, 1.0f, 0.0f, // bottom lelf
        0.0f, 0.0f, 1.0f, // top
    };


    // static GLfloat scale = 0.0;
    // static GLfloat delta = 0.15;

    // scale += delta;
    // if (scale >= 1.0 || scale <= 1.0) {
    //     delta *= -1.0;
    // }
    glUseProgram(program_);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate Vertext Buffer Object for vertex
    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    // Bind data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), nullptr);
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    // Generate Vertext Buffer Object for color
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    // color attribute
    glVertexAttribPointer(ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), nullptr);
    glEnableVertexAttribArray(ATTRIB_COLOR);

    // glUniform1f(scale_, scale);
    glUniformMatrix4fv(transform_, 1, GL_FALSE, glm::value_ptr(transform));

    return absl::OkStatus();
}

absl::Status Gl09TriangleInterpolationCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                              double timestamp) {
    // drawring 
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    return absl::OkStatus();    
}

absl::Status Gl09TriangleInterpolationCalculator::GlCleanup() {
    // cleanup
    glDisableVertexAttribArray(ATTRIB_VERTEX);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(2, VBO);

    return absl::OkStatus();    
}

absl::Status Gl09TriangleInterpolationCalculator::GlTeardown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    return absl::OkStatus();
}

} // mediapipe