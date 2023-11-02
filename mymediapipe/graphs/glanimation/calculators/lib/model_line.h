#ifndef MODEL_LINE_H
#define MODEL_LINE_H

class ModelLine {
public:
    ModelLine() {}
    ~ModelLine() {}

    void Setup() {
        const GLchar* vert_src = 
        R"(
            attribute vec3 position;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;

            void main()
            {
                gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
            }
        )";

        const GLchar* frag_src = 
        R"(
            precision mediump float;

            void main()
            {
                gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
            } 
        )";

        ourShader = new Shader(vert_src, frag_src);

        // Initial potition of dot
        // GLfloat vertices[] = {
        //     // -1.0f, -1.0f, 0.0f,
        //     // 1.0f, -1.0f, 0.0f,
        //     // 0.0f,  1.0f, 0.0f,
        //     // 0.008416f, -0.634527f, -0.260950f, // nose
        //     // 0.023932f, -0.649963f, -0.246623f, // left eye inner
        //     // 0.021935f, -0.652791f, -0.236793f, // left eye
        //     // 0.020602f, -0.652721f, -0.238632f, // right eye inner
        //     // -0.001918f, -0.657420f, -0.264558f, // right eye
        //     // -0.000328f, -0.658103f, -0.277128f, // left ear
        //     // 0.003014f, -0.644046f, -0.256564f, // right ear
        //     // 0.074619f, -0.637829f, -0.145254f, // 
        //     // -0.067449f, -0.582788f, -0.155069f, // 
        //     // 0.042210f, -0.618381f, -0.213900f, 
        //     // 0.003060f, -0.583580f, -0.246246f,
        //     0.167898f, 1.0 - -0.529274f, -0.047496f * -1.0, // 11. right arm
        //     // -0.118491f, -0.578913f, -0.044215f, // 12. left arm
        //     0.295250f, 1.0 - -0.692503f, -0.095195f * -1.0, // 13. right elbow
        //     // -0.226583f, -0.653058f, -0.189032f, // 14. left elbow
        //     0.388551f, 1.0 - -0.875161f, -0.130593f * -1.0, // 15. right hand
        //     // -0.324218f, -0.868572f, -0.289152f, // 16. left hand
        //     // 0.396092f, -0.923208f, -0.140909f, // 17. right pinky
        //     // -0.326189f, -0.924808f, -0.353246f, // 18.
        //     // 0.378265f, -0.947431f, -0.157319f, // 19.
        //     // -0.294475f, -0.980209f, -0.352735f, // 20.
        //     // 0.389180f, -0.872154f, -0.144568f, // 21.
        //     // -0.318777f, -0.897202f, -0.299864f, // 22.
        //     0.084799f, 1.0 - -0.005909f, 0.027898f * -1.0, // 23. right hip
        //     // -0.083190f, 0.001052f, -0.025328f, // 24. left hip
        //     0.078926f, 1.0 - 0.383440f, 0.000040f * -1.0, // 25. right knee
        //     // -0.046097f, 0.336393f, -0.012527f, // 26. leff knee
        //     0.061945f, 1.0 - 0.736835f, 0.103868f * -1.0, // 27. right ankle
        //     // -0.085045f, 0.703524f, 0.142972f, // 28. left ankle
        //     // 0.035896f, 0.770580f, 0.119718f, // 29. 
        //     // -0.089242f, 0.737073f, 0.120542f, // 30.
        //     // 0.031568f, 0.807700f, 0.066122f, // 31.
        //     // -0.126434f, 0.764138f, 0.051662f, // 32.
        //     // 0.000804f, -0.002429f, 0.001285f,
        //     // 0.024704f, -0.554094f, -0.045855f,
        //     // 0.000804f, -0.002429f, 0.001285f
        //     -0.118491f, 1.0 - -0.578913f, -0.044215f * -1.0, // left arm
        //     -0.226583f, 1.0 - -0.653058f, -0.189032f * -1.0, // left elbow
        //     -0.324218f, 1.0 - -0.868572f, -0.289152f * -1.0, // left hand
        //     -0.083190f, 1.0 - 0.001052f, -0.025328f * -1.0, // 24. left hip
        //     -0.046097f, 1.0 - 0.336393f, -0.012527f * -1.0, // 26. leff knee
        //     -0.085045f, 1.0 - 0.703524f, 0.142972f * -1.0, // 28. left ankle
        // };

        // glGenVertexArrays(1, &VAO);
        // glBindVertexArray(VAO);

        // // Generate Vertext Buffer Object
        // glGenBuffers(1, &VBO);
        // glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // // Bind data
        // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // glEnableVertexAttribArray(0);
        // glBindBuffer(GL_ARRAY_BUFFER, VBO); 
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    }

    void Draw(const FrameBufferTarget &framebuffer_target_, 
              int src_width, int src_height,
              double deltaTime,
              std::vector<glm::vec3> &landmarks) {

        // glm::vec3 lm3_11 = glm::vec3(0.167898f, -0.529274f, -0.047496f);
        // glm::vec3 lm3_13 = glm::vec3(0.295250f, -0.692503f, -0.095195f); // 13. right elbow
        // glm::vec3 lm3_15 = glm::vec3(0.388551f, -0.875161f, -0.130593f); // 15. right hand
        // glm::vec3 lm3_12 = glm::vec3(-0.118491f, -0.578913f, -0.044215f); // left arm
        // glm::vec3 lm3_14 = glm::vec3(-0.226583f, -0.653058f, -0.189032f); // left elbow
        // glm::vec3 lm3_16 = glm::vec3(-0.324218f, -0.868572f, -0.289152f); // left hand

        // GLfloat vertices[] = {
        //     // -1.0f, -1.0f, 0.0f,
        //     // 1.0f, -1.0f, 0.0f,
        //     // 0.0f,  1.0f, 0.0f,
        //     // 0.008416f, -0.634527f, -0.260950f, // nose
        //     // 0.023932f, -0.649963f, -0.246623f, // left eye inner
        //     // 0.021935f, -0.652791f, -0.236793f, // left eye
        //     // 0.020602f, -0.652721f, -0.238632f, // right eye inner
        //     // -0.001918f, -0.657420f, -0.264558f, // right eye
        //     // -0.000328f, -0.658103f, -0.277128f, // left ear
        //     // 0.003014f, -0.644046f, -0.256564f, // right ear
        //     // 0.074619f, -0.637829f, -0.145254f, // 
        //     // -0.067449f, -0.582788f, -0.155069f, // 
        //     // 0.042210f, -0.618381f, -0.213900f, 
        //     // 0.003060f, -0.583580f, -0.246246f,
        //     lm3_11.x, lm3_11.y * -1, lm3_11.z * -1, // 11. right arm
        //     // -0.118491f, -0.578913f, -0.044215f, // 12. left arm
        //     // 0.295250f, -0.692503f, -0.095195f * -1.0, // 13. right elbow
        //     lm3_13.x, lm3_13.y * -1, lm3_13.z * -1,
        //     // -0.226583f, -0.653058f, -0.189032f, // 14. left elbow
        //     // 0.388551f, -0.875161f, -0.130593f * -1.0, // 15. right hand
        //     lm3_15.x, lm3_15.y * -1, lm3_15.z * -1,
        //     // -0.324218f, -0.868572f, -0.289152f, // 16. left hand
        //     // 0.396092f, -0.923208f, -0.140909f, // 17. right pinky
        //     // -0.326189f, -0.924808f, -0.353246f, // 18.
        //     // 0.378265f, -0.947431f, -0.157319f, // 19.
        //     // -0.294475f, -0.980209f, -0.352735f, // 20.
        //     // 0.389180f, -0.872154f, -0.144568f, // 21.
        //     // -0.318777f, -0.897202f, -0.299864f, // 22.
        //     0.084799f, -0.005909f * -1.0, 0.027898f * -1.0, // 23. right hip
        //     // -0.083190f, 0.001052f, -0.025328f, // 24. left hip
        //     0.078926f, 0.383440f * -1.0, 0.000040f * -1.0, // 25. right knee
        //     // -0.046097f, 0.336393f, -0.012527f, // 26. leff knee
        //     0.061945f, 0.736835f * -1.0, 0.103868f * -1.0, // 27. right ankle
        //     // -0.085045f, 0.703524f, 0.142972f, // 28. left ankle
        //     // 0.035896f, 0.770580f, 0.119718f, // 29. 
        //     // -0.089242f, 0.737073f, 0.120542f, // 30.
        //     // 0.031568f, 0.807700f, 0.066122f, // 31.
        //     // -0.126434f, 0.764138f, 0.051662f, // 32.
        //     // 0.000804f, -0.002429f, 0.001285f,
        //     // 0.024704f, -0.554094f, -0.045855f,
        //     // 0.000804f, -0.002429f, 0.001285f
        //     // -0.118491f, -0.578913f, -0.044215f * -1.0, // left arm
        //     // -0.226583f, -0.653058f, -0.189032f * -1.0, // left elbow
        //     // -0.324218f, -0.868572f, -0.289152f * -1.0, // left hand
        //     lm3_12.x, lm3_12.y * -1, lm3_12.z * -1,
        //     lm3_14.x, lm3_14.y * -1, lm3_14.z * -1,
        //     lm3_16.x, lm3_16.y * -1, lm3_16.z * -1,
        //     -0.083190f, 0.001052f * -1.0, -0.025328f * -1.0, // 24. left hip
        //     -0.046097f, 0.336393f * -1.0, -0.012527f * -1.0, // 26. leff knee
        //     -0.085045f, 0.703524f * -1.0, 0.142972f * -1.0, // 28. left ankle
        // };

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate Vertext Buffer Object
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Bind data
        glBufferData(GL_ARRAY_BUFFER, landmarks.size() * sizeof(glm::vec3), landmarks.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO); 
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);


        ourShader->use();
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, -1.0f, 0.0f);
        // glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(65.f), (float)src_width / (float)src_height, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(camPos, position, cameraUp);
        ourShader->setMat4("projection", projection);
        ourShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float) deltaTime * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader->setMat4("model", model);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        framebuffer_target_.Bind();
        glViewport(0, 0, src_width, src_height);

        ourShader->use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, landmarks.size());
        // glDrawArrays(GL_LINES, 0, 33);
        glBindVertexArray(0);

        framebuffer_target_.Unbind();
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    }

private:
    Shader *ourShader;
    GLuint VBO, VAO, EBO;
};

#endif