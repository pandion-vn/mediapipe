#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "gl_base_calculator.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"


namespace mediapipe {

enum { ATTRIB_VERTEX, ATTRIB_COLOR, ATTRIB_TEXTURE_COORDS, NUM_ATTRIBUTES };
enum { M_TEXTURE0, M_TEXTURE1, M_TEXTURE2, M_TEXTURE3, NUM_TEXTURES};

// https://learnopengl.com/Getting-started/Textures
class Gl10TriangleTextureCalculator : public GlBaseCalculator {
public:
    absl::Status GlSetup() override;
    absl::Status GlBind() override;
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) override;
    absl::Status GlCleanup() override;
    absl::Status GlTeardown() override;
private:
    GLuint program_ = 0;
    GLint texture0_, texture1_, texture2_;
    GLint scale_, transform_;
    GLuint VBO[3], VAO, EBO, TextureBO[2];
};

REGISTER_CALCULATOR(Gl10TriangleTextureCalculator);

absl::Status Gl10TriangleTextureCalculator::GlSetup() {

    // Load vertex and fragment shaders
    const GLint attr_location[NUM_ATTRIBUTES] = {
        ATTRIB_VERTEX,
        ATTRIB_COLOR,
        ATTRIB_TEXTURE_COORDS,
    };

    const GLchar* attr_name[NUM_ATTRIBUTES] = {
        "position",
        "color",
        "tex_coord",
    };

    const GLchar* vert_src = R"(
        attribute vec3 position;
        attribute vec3 color;
        attribute vec2 tex_coord;

        // uniform float gScale;
        uniform mat4 transform;
        varying vec3 ourColor;
        varying vec2 ourTexCoord;

        void main()
        {
            gl_Position = transform * vec4(position, 1.0);
            ourColor = color;
            ourTexCoord = tex_coord;
        }
    )";

    const GLchar* frag_src = R"(
        precision mediump float;
        varying vec3 ourColor;
        varying vec2 ourTexCoord;
        uniform sampler2D texture0;
        uniform sampler2D texture1;
        uniform sampler2D texture2;

        void main()
        {
            // gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
            // gl_FragColor = vec4(ourColor, 1.0);
            // gl_FragColor = texture2D(ourTexture, ourTexCoord) * vec4(ourColor, 1.0);
            vec4 color = mix(texture2D(texture0, ourTexCoord), texture2D(texture1, ourTexCoord), 0.4);
            vec4 color1 = mix(color, texture2D(texture2, ourTexCoord), 0.2);
            gl_FragColor = color1 * vec4(ourColor, 1.0);
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
    texture0_ = glGetUniformLocation(program_, "texture0");
    RET_CHECK_NE(texture0_, -1) << "Failed to find `texture` uniform!";
    texture1_ = glGetUniformLocation(program_, "texture1");
    RET_CHECK_NE(texture1_, -1) << "Failed to find `texture` uniform!";
    texture2_ = glGetUniformLocation(program_, "texture2");
    RET_CHECK_NE(texture2_, -1) << "Failed to find `texture` uniform!";

    // generate texture
    glGenTextures(2, TextureBO);
    glBindTexture(GL_TEXTURE_2D, TextureBO[0]);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("mymediapipe/assets/opengl/question-mark.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Loaded texture" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, TextureBO[1]);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width1, height1, nrChannels1;
    unsigned char *data1 = stbi_load("mymediapipe/assets/opengl/awesomeface.png", &width1, &height1, &nrChannels1, 0);
    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Loaded texture" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data1);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "DONE setup" << std::endl;
    return absl::OkStatus();
}

absl::Status Gl10TriangleTextureCalculator::GlBind() {

    glm::mat4 transform = glm::mat4(1.0);
    // transform = glm::translate(transform, glm::vec3(0.3, -0.3, 0.0));

    // Initial potition of dot
    GLfloat vertices[] = {
        // positions      
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,  // top left
    };

    GLfloat colors[] = {
        // colors
        1.0f, 0.0f, 0.0f, // top right
        0.0f, 1.0f, 0.0f, // bottom right
        0.0f, 0.0f, 1.0f, // bottom left
        1.0f, 1.0f, 0.0f, // top left
    };

    GLfloat texture_coords[] = {
        // texture coords square
        1.0f, 1.0f,     // top right
        1.0f, 0.0f,     // bottom right
        0.0f, 0.0f,     // bottom left
        0.0f, 1.0f,     // top left
        // 1.0f, 0.0f, // lower-right corner
        // 0.0f, 0.0f, // lower-left corner
        // 0.5f, 1.0f, // top-center corner
    };

    GLuint indices[] = {
        0, 1, 3,        // first triangle
        // 1, 2, 3,        // second triangle
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
    glGenBuffers(3, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    // Bind data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // vertex position attribute
    glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), nullptr);
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    // Generate indices bufer object for element array
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate Vertext Buffer Object for color
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    // color attribute
    glVertexAttribPointer(ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), nullptr);
    glEnableVertexAttribArray(ATTRIB_COLOR);

    // Generate Vertex Buffer Object for texture coords
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coords), texture_coords, GL_STATIC_DRAW);
    // texture coord attribute
    glVertexAttribPointer(ATTRIB_TEXTURE_COORDS, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(float), nullptr);
    glEnableVertexAttribArray(ATTRIB_TEXTURE_COORDS);

    // glUniform1f(scale_, scale);
    glUniformMatrix4fv(transform_, 1, GL_FALSE, glm::value_ptr(transform));
    glUniform1i(texture0_, M_TEXTURE0);
    glUniform1i(texture1_, M_TEXTURE1);
    glUniform1i(texture2_, M_TEXTURE2);

    return absl::OkStatus();
}

absl::Status Gl10TriangleTextureCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                              double timestamp) {
    // drawring 
    glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, TextureBO[0]);
    glBindTexture(src.target(), src.name());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, TextureBO[0]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, TextureBO[1]);
    // glBindTexture(src.target(), src.name());

    glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // std::cout << "glDrawArrays texture" << std::endl;
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // unbind VAO
    glBindVertexArray(0);

    // unbind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);


    return absl::OkStatus();    
}

absl::Status Gl10TriangleTextureCalculator::GlCleanup() {
    // cleanup
    glDisableVertexAttribArray(ATTRIB_VERTEX);
    glDisableVertexAttribArray(ATTRIB_COLOR);
    glDisableVertexAttribArray(ATTRIB_TEXTURE_COORDS);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(3, VBO);
    glDeleteBuffers(1, &EBO);

    return absl::OkStatus();    
}

absl::Status Gl10TriangleTextureCalculator::GlTeardown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
    return absl::OkStatus();
}

} // mediapipe