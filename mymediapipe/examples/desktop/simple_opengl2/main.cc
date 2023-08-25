#include <cstdlib>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "include/common.h"
#include "include/sandbox.h"

Sandbox* sandboxApp;

mediapipe::Status RunMPPGraph() {
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

    mediapipe::Status status = Sandbox::InitialMPPGraph();
    if (!status.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << status.message();
        return mediapipe::Status(mediapipe::StatusCode::kInternal, "Failed to run the graph");
    } else {
        LOG(INFO) << "Success!";
    }

    GLuint vao;
	GLuint vbo;
	/* geometry to use. these are 3 xyz points (9 floats total) to make a triangle
	*/
	GLfloat points[] = {
		 0.0f,	0.5f,	0.0f,
		 0.5f, -0.5f,	0.0f,
		-0.5f, -0.5f,	0.0f
	};
	/* these are the strings of code for the shaders
	the vertex shader positions each vertex point */
	const char* vertex_shader =
	"#version 410\n"
	"in vec3 vp;"
	"void main () {"
	"	gl_Position = vec4 (vp, 1.0);"
	"}";
	/* the fragment shader colours each fragment (pixel-sized area of the
	triangle) */
	const char* fragment_shader =
	"#version 410\n"
	"out vec4 frag_colour;"
	"void main () {"
	"	frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
	"}";
	/* GL shader objects for vertex and fragment shader [components] */
	GLuint vs, fs;
	/* GL shader programme object [combined, to link] */
	GLuint shader_programme;

    /* a vertex buffer object (VBO) is created here. this stores an array of data
	on the graphics adapter's memory. in our case - the vertex points */
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (GLfloat), points, GL_STATIC_DRAW);
	
	/* the vertex array object (VAO) is a little descriptor that defines which
	data from vertex buffer objects should be used as input variables to vertex
	shaders. in our case - use our only VBO, and say 'every three floats is a 
	variable' */
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	/* here we copy the shader strings into GL shaders, and compile them. we then
	create an executable shader 'program' and attach both of the compiled shaders.
	we link this, which matches the outputs of the vertex shader to the inputs of
	the fragment shader, etc. and it is then ready to use */
	vs = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vs, 1, &vertex_shader, NULL);
	glCompileShader (vs);
	fs = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (fs, 1, &fragment_shader, NULL);
	glCompileShader (fs);
	shader_programme = glCreateProgram ();
	glAttachShader (shader_programme, fs);
	glAttachShader (shader_programme, vs);
	glLinkProgram (shader_programme);

    while (!glfwWindowShouldClose(Sandbox::glfwWindow)) {
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        sandboxApp->Render(currentFrame);
        glUseProgram (shader_programme);
		glBindVertexArray (vao);
		/* draw points 0-3 from the currently bound VAO with current in-use shader*/
		glDrawArrays (GL_TRIANGLES, 0, 3);

        frames++;
        glfwSwapBuffers(Sandbox::glfwWindow);
        glfwPollEvents();
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
    MP_RETURN_IF_ERROR(Sandbox::graph.CloseInputStream(kInputStream));
    // return Sandbox::graph.WaitUntilDone();
    return mediapipe::OkStatus();
}
    
int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    absl::ParseCommandLine(argc, argv);
    mediapipe::Status runStatus = RunMPPGraph();
  
    if (!runStatus.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << runStatus.message();
        return EXIT_FAILURE;
    } else {
        LOG(INFO) << "Success!";
    }
    return EXIT_SUCCESS;
}