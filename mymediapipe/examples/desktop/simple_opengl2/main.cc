#include <cstdlib>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "include/common.h"
#include "include/sandbox.h"

Sandbox* sandboxApp;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


mediapipe::Status RunMPPGraph() {
    // mediapipe::Status status = Sandbox::InitialMPPGraph();
    // if (!status.ok()) {
    //     LOG(ERROR) << "Failed to run the graph: " << status.message();
    //     return mediapipe::Status(mediapipe::StatusCode::kInternal, "Failed to run the graph");
    // } else {
    //     LOG(INFO) << "Success!";
    // }
    Sandbox::InitOpenGL();

    // Debugger FramebufferSize
    int viewport_w, viewport_h;
    float viewport_aspect;
    glfwGetFramebufferSize(Sandbox::glfwWindow, &viewport_w, &viewport_h);
    viewport_aspect = float(viewport_h) / float(viewport_w);
    LOG(INFO) << "viewport_w: " << viewport_w <<  
                 " , viewport_h: " << viewport_h << 
                 " , aspect: " << viewport_aspect;

    srand(time(NULL));
    
    sandboxApp = new Sandbox(viewport_w, viewport_h);
    Sandbox::ConfigureOpenGL(viewport_w, viewport_h);
    sandboxApp->Init();
    Sandbox::InitCamCapture();
    // deltaTime variables
    // -------------------
    static double limitFPS = 1.0 / 30.0;
    float deltaTime = 0.0f;
    float fps = 0.0f;
    float lastFrame = glfwGetTime(), timer = lastFrame;
    int32_t frames = 0, updates = 0;

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    while (!glfwWindowShouldClose(Sandbox::glfwWindow)) {
        glfwPollEvents();

        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        fps += deltaTime / limitFPS;
        lastFrame = currentFrame;

        while (fps >= 1.0) {
            // manage user input
            // -----------------
            sandboxApp->ProcessInput(deltaTime);

            // update game state
            // -----------------
            sandboxApp->Update(deltaTime);
            updates++;
            fps--;
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // sandboxApp->Render(currentFrame);
        frames++;
        glfwSwapBuffers(Sandbox::glfwWindow);
        if (glfwGetTime() - timer > 1.0) {
            timer++;
            LOG(INFO) << "FPS: " << frames << ", updates:" << updates;
            updates = 0, frames = 0;
        }
    }
    sandboxApp->stop();
    sandboxApp->join();

    glfwDestroyWindow(Sandbox::glfwWindow);
    glfwTerminate();
    LOG(INFO) << "Shutting down.";
    // MP_RETURN_IF_ERROR(Sandbox::graph.CloseInputStream(kInputStream));
    // return Sandbox::graph.WaitUntilDone();
    return mediapipe::OkStatus();
}
    
int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    // absl::ParseCommandLine(argc, argv);
    mediapipe::Status runStatus = RunMPPGraph();
  
    if (!runStatus.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << runStatus.message();
        return EXIT_FAILURE;
    } else {
        LOG(INFO) << "Success!";
    }
    return EXIT_SUCCESS;
}