#ifndef VIDEO_SCENE_H
#define VIDEO_SCENE_H

#include "shader.h"

class VideoScene {
public:
    VideoScene() {}

    void Setup() {
        static const GLfloat square_vertices[] = {
            // positions      
            // -1.0f, -1.0f, 0.0f,  // TOP left
            // 1.0f, -1.0f, 0.0f,  // TOP right
            // -1.0f,  1.0f, 0.0f,  // BOTTOM left
            // 1.0f,  1.0f, 0.0f,  // BOTTOM right
            // bottom right position
            // 0.5f, -1.0f, 0.0f,  // TOP left
            // 1.0f, -1.0f, 0.0f,  // TOP right
            // .5f,  -.5f, 0.0f,  // BOTTOM left
            // 1.0f, -.5f, 0.0f,  // BOTTOM right

            0.5f, 0.5f, 0.0f,  // half TOP left
            1.0f, 0.5f, 0.0f,  // half TOP right
            0.5f, 1.0f, 0.0f,  // half BOTTOM left
            1.0f, 1.0f, 0.0f,  // half BOTTOM right

            // 0.0f, 0.0f, 0.0f,  // zero TOP left
            // 1.0f, 0.0f, 0.0f,  // zero TOP right
            // 0.0f, 1.0f, 0.0f,  // zero BOTTOM left
            // 1.0f, 1.0f, 0.0f,  // BOTTOM right
        };
        static const float texture_vertices[] = {
            // texture coords square
            0.0f, 0.0f,     // top left
            1.0f, 0.0f,     // top right
            0.0f, 1.0f,     // BOTTOM left
            1.0f, 1.0f,     // BOTTOM right
        };

        // vertex storage
        //   GLuint vbo[2];
        glGenBuffers(2, VBO);
        //   GLuint vao;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // vbo 0
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // vbo 1
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texture_vertices), texture_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        cameraShader = new Shader(camera_vert_src, camera_frag_src);
    }

    void Draw(const FrameBufferTarget &framebuffer_target_, GLenum src_target, GLuint src_name, int src_width, int src_height) {
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        framebuffer_target_.Bind();
        glViewport(0, 0, src_width, src_height);
        cameraShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(src_target, src_name);
        cameraShader->setInt("texture0", 0);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        framebuffer_target_.Unbind();
        
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

private:

    const GLchar* camera_vert_src = R"(
        #version 310 es
        precision highp float;
        in vec3 position;
        in vec2 tex_coord;

        out vec2 ourTexCoord;

        void main()
        {
            ourTexCoord = tex_coord;
            gl_Position = vec4(position, 1.0);
        }
    )";

    const GLchar* camera_frag_src = R"(
        #version 310 es
        precision highp float;
        out vec4 fragColor;
        in vec2 ourTexCoord;
        uniform sampler2D texture0;

        void main()
        {
            fragColor = vec4(texture(texture0, ourTexCoord).rgb, 0.5);
        } 
    )";

    Shader *cameraShader;
    GLuint VBO[2], VAO;
};

#endif