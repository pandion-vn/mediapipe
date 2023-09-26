#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "gl_base_calculator.h"

namespace mediapipe {

enum { ATTRIB_VERTEX, NUM_ATTRIBUTES };

class Gl03TriangleCalculator : public GlBaseCalculator {
public:
    absl::Status GlSetup() override;
    absl::Status GlBind() override;
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) override;
    absl::Status GlCleanup() override;
    absl::Status GlTeardown() override;
private:
    GLuint program_ = 0;
    GLint texture_;
    GLuint VBO, VAO, EBO;
};

REGISTER_CALCULATOR(Gl03TriangleCalculator);

absl::Status Gl03TriangleCalculator::GlSetup() {

    // Load vertex and fragment shaders
    const GLint attr_location[NUM_ATTRIBUTES] = {
        ATTRIB_VERTEX,
    };

    const GLchar* attr_name[NUM_ATTRIBUTES] = {
        "position",
    };

    const GLchar* vert_src = R"(
        attribute vec3 position;

        void main()
        {
            gl_Position = vec4(position.x, position.y, position.z, 1.0);
        }
    )";

    const GLchar* frag_src = R"(
        precision mediump float;

        void main()
        {
            gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        } 
    )";

    // shader program
    GlhCreateProgram(vert_src, frag_src, NUM_ATTRIBUTES,
                    (const GLchar**)&attr_name[0], attr_location, &program_);
    RET_CHECK(program_) << "Problem initializing the program.";

    return absl::OkStatus();
}

absl::Status Gl03TriangleCalculator::GlBind() {

    // Initial potition of dot
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f,
    };

    // Generate Vertext Buffer Object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Bind data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); 

    glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    return absl::OkStatus();
}

absl::Status Gl03TriangleCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                              double timestamp) {
    // drawring 
    glUseProgram(program_);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return absl::OkStatus();    
}

absl::Status Gl03TriangleCalculator::GlCleanup() {
    // cleanup
    glDisableVertexAttribArray(ATTRIB_VERTEX);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    return absl::OkStatus();    
}

absl::Status Gl03TriangleCalculator::GlTeardown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    return absl::OkStatus();
}

} // mediapipe