#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "gl_base_calculator.h"

namespace mediapipe {

enum { ATTRIB_VERTEX, NUM_ATTRIBUTES };

class Gl02DotCalculator : public GlBaseCalculator {
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

REGISTER_CALCULATOR(Gl02DotCalculator);

absl::Status Gl02DotCalculator::GlSetup() {
    return absl::OkStatus();
}

absl::Status Gl02DotCalculator::GlBind() {

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

absl::Status Gl02DotCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                          double timestamp) {
    // drawring 
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return absl::OkStatus();    
}

absl::Status Gl02DotCalculator::GlCleanup() {
    // cleanup
    glDisableVertexAttribArray(ATTRIB_VERTEX);

    return absl::OkStatus();    
}

absl::Status Gl02DotCalculator::GlTeardown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    return absl::OkStatus();
}

} // mediapipe