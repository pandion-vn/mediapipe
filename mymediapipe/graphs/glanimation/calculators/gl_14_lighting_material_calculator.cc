#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/gpu/gl_simple_shaders.h"
#include "mediapipe/gpu/shader_util.h"
#include "gl_base_calculator.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "lib/stb_image.h"
#include "lib/shader.h"
#include "lib/camera.h"

namespace mediapipe {

enum { LIGHTING_ATTRIB_VERTEX, LIGHTING_ATTRIB_NORMAL, LIGHTING_NUM_ATTRIBUTES };
enum { CUBE_ATTRIB_VERTEX, CUBE_NUM_ATTRIBUTES };
enum { M_TEXTURE0, M_TEXTURE1, M_TEXTURE2, M_TEXTURE3, NUM_TEXTURES };

// https://learnopengl.com/Lighting/Materials
class Gl14LightingMaterialCalculator : public GlBaseCalculator {
public:
    absl::Status GlSetup() override;
    absl::Status GlBind() override;
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) override;
    absl::Status GlCleanup() override;
    absl::Status GlTeardown() override;
private:
    // GLuint lightingShader_ = 0;
    // GLuint cubeShader_ = 0;

    // GLint texture0_, texture1_, texture2_;
    // GLint model_, view_, projection_;
    GLuint VBO[2], cubeVAO, lightCubeVAO, EBO, TextureBO[2];
    // GLint lightingPosition_, lightingViewPos_, lightingAmbient_,
    //       lightingDiffuse_, lightingSpecular_, lightingShininess_,
    //       lightingView_, lightingProjection_, lightingModel_;
    // GLint materialAmbient_, materialDiffuse_, materialSpecular_, materialShininess_;
    // GLint cubeView_, cubeProjection_, cubeModel_;
    Shader *lightingShader, *lightCubeShader;
};

REGISTER_CALCULATOR(Gl14LightingMaterialCalculator);

absl::Status Gl14LightingMaterialCalculator::GlSetup() {

    // Load vertex and fragment shaders
    // const GLint lighting_attr_location[LIGHTING_NUM_ATTRIBUTES] = {
    //     LIGHTING_ATTRIB_VERTEX,
    //     LIGHTING_ATTRIB_NORMAL,
    // };

    // const GLchar* lighting_attr_name[LIGHTING_NUM_ATTRIBUTES] = {
    //     "position",
    //     "normal",
    // };

    const GLchar* lighting_vert_src = 
    R"(
        #version 310 es
        precision mediump float;

        in vec3 position;
        in vec3 normal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 outFragPos;
        out vec3 outNormal;

        void main()
        {
            outFragPos = vec3(model * vec4(position, 1.0));
            outNormal = mat3(transpose(inverse(model))) * normal;
            // outNormal = vec3(normal);
            
            gl_Position = projection * view * vec4(outFragPos, 1.0);
        }
    )";

    const GLchar* lighting_frag_src = 
    R"(
        #version 310 es
        precision mediump float;

        out vec4 fragColor;

        struct Material {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            float shininess;
        }; 

        struct Light {
            vec3 position;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
        };

        in vec3 outFragPos;
        in vec3 outNormal;

        // uniform sampler2D texture0;
        uniform vec3 viewPos;
        uniform Material material;
        uniform Light light;

        void main()
        {
            // ambient
            vec3 ambient = light.ambient * material.ambient;
            
            // diffuse 
            vec3 norm = normalize(outNormal);
            vec3 lightDir = normalize(light.position - outFragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = light.diffuse * (diff * material.diffuse);
            
            // specular
            vec3 viewDir = normalize(viewPos - outFragPos);
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            vec3 specular = light.specular * (spec * material.specular);  
                
            vec3 result = ambient + diffuse + specular;
            // vec3 result = ambient + diffuse;

            fragColor = vec4(result, 1.0);
        } 
    )";

    // Load vertex and fragment shaders
    // const GLint cube_attr_location[CUBE_NUM_ATTRIBUTES] = {
    //     CUBE_ATTRIB_VERTEX,
    // };

    // const GLchar* cube_attr_name[CUBE_NUM_ATTRIBUTES] = {
    //     "position",
    // };

    const GLchar* cube_vert_src = R"(
        #version 310 es
        in vec3 position;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {            
            gl_Position = projection * view * model * vec4(position, 1.0);
        }
    )";

    const GLchar* cube_frag_src = R"(
        #version 310 es
        precision mediump float;
        out vec4 fragColor;

        void main()
        {
            fragColor = vec4(1.0);
        } 
    )";

    lightingShader = new Shader(lighting_vert_src, lighting_frag_src);
    lightCubeShader = new Shader(cube_vert_src, cube_frag_src);
    // shader program
    // GlhCreateProgram(lighting_vert_src, lighting_frag_src, LIGHTING_NUM_ATTRIBUTES,
    //                 (const GLchar**)&lighting_attr_name[0], lighting_attr_location, &lightingShader_);
    // RET_CHECK(lightingShader_) << " Problem initializing the program.";

    // shader program
    // GlhCreateProgram(cube_vert_src, cube_frag_src, CUBE_NUM_ATTRIBUTES,
    //                 (const GLchar**)&cube_attr_name[0], cube_attr_location, &cubeShader_);
    // RET_CHECK(cubeShader_) << " Problem initializing the program.";

    // lightingViewPos_ = glGetUniformLocation(lightingShader_, "viewPos");
    // RET_CHECK_NE(lightingViewPos_, -1) << "Failed to find `viewPos` uniform!";
    
    // lightingPosition_ = glGetUniformLocation(lightingShader_, "light.position");
    // RET_CHECK_NE(lightingPosition_, -1) << "Failed to find `light.position` uniform!";

    // lightingAmbient_ = glGetUniformLocation(lightingShader_, "light.ambient");
    // RET_CHECK_NE(lightingAmbient_, -1) << "Failed to find `light.ambient` uniform!";

    // lightingDiffuse_ = glGetUniformLocation(lightingShader_, "light.diffuse");
    // RET_CHECK_NE(lightingDiffuse_, -1) << "Failed to find `light.diffuse` uniform!";

    // lightingSpecular_ = glGetUniformLocation(lightingShader_, "light.specular");
    // RET_CHECK_NE(lightingSpecular_, -1) << "Failed to find `light.specular` uniform!";

    // lightingView_ = glGetUniformLocation(lightingShader_, "view");
    // RET_CHECK_NE(lightingView_, -1) << "Failed to find `view` uniform!";

    // lightingProjection_ = glGetUniformLocation(lightingShader_, "projection");
    // RET_CHECK_NE(lightingProjection_, -1) << "Failed to find `projection` uniform!";

    // lightingModel_ = glGetUniformLocation(lightingShader_, "model");
    // RET_CHECK_NE(lightingModel_, -1) << "Failed to find `projection` uniform!";

    // materialAmbient_ = glGetUniformLocation(lightingShader_, "material.ambient");
    // RET_CHECK_NE(materialAmbient_, -1) << "Failed to find `material.ambient` uniform!";

    // materialDiffuse_ = glGetUniformLocation(lightingShader_, "material.diffuse");
    // RET_CHECK_NE(materialDiffuse_, -1) << "Failed to find `material.diffuse` uniform!";

    // materialSpecular_ = glGetUniformLocation(lightingShader_, "material.specular");
    // RET_CHECK_NE(materialSpecular_, -1) << "Failed to find `material.specular` uniform!";

    // materialShininess_ = glGetUniformLocation(lightingShader_, "material.shininess");
    // RET_CHECK_NE(materialShininess_, -1) << "Failed to find `material.shininess` uniform!";

    // cubeView_ = glGetUniformLocation(cubeShader_, "view");
    // RET_CHECK_NE(cubeView_, -1) << "Failed to find `view` uniform!";

    // cubeProjection_ = glGetUniformLocation(cubeShader_, "projection");
    // RET_CHECK_NE(cubeProjection_, -1) << "Failed to find `projection` uniform!";

    // cubeModel_ = glGetUniformLocation(cubeShader_, "model");
    // RET_CHECK_NE(cubeModel_, -1) << "Failed to find `projection` uniform!";

    // texture0_ = glGetUniformLocation(program_, "texture0");
    // RET_CHECK_NE(texture0_, -1) << "Failed to find `texture` uniform!";
    // texture1_ = glGetUniformLocation(program_, "texture1");
    // RET_CHECK_NE(texture1_, -1) << "Failed to find `texture` uniform!";
    // texture2_ = glGetUniformLocation(program_, "texture2");
    // RET_CHECK_NE(texture2_, -1) << "Failed to find `texture` uniform!";

    // // generate texture
    // glGenTextures(2, TextureBO);
    // glBindTexture(GL_TEXTURE_2D, TextureBO[0]);
    // // set the texture wrapping/filtering options (on the currently bound texture object)
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // // load and generate the texture
    // int width, height, nrChannels;
    // unsigned char *data = stbi_load("mymediapipe/assets/opengl/question-mark.png", &width, &height, &nrChannels, 0);
    // if (data)
    // {
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    //     std::cout << "Loaded texture" << std::endl;
    // }
    // else
    // {
    //     std::cout << "Failed to load texture" << std::endl;
    // }
    // stbi_image_free(data);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // glBindTexture(GL_TEXTURE_2D, TextureBO[1]);
    // // set the texture wrapping/filtering options (on the currently bound texture object)
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // // load and generate the texture
    // int width1, height1, nrChannels1;
    // unsigned char *data1 = stbi_load("mymediapipe/assets/opengl/awesomeface.png", &width1, &height1, &nrChannels1, 0);
    // if (data1)
    // {
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    //     std::cout << "Loaded texture" << std::endl;
    // }
    // else
    // {
    //     std::cout << "Failed to load texture" << std::endl;
    // }
    // stbi_image_free(data1);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    std::cout << "DONE setup" << std::endl;
    return absl::OkStatus();
}

absl::Status Gl14LightingMaterialCalculator::GlBind() {

    // Initial postition of cube
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    GLfloat normals[] = {
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
    };

    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // Generate Vertext Buffer Object for vertex
    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    // Bind data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // vertex position attribute
    glVertexAttribPointer(LIGHTING_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(LIGHTING_ATTRIB_VERTEX);

    // Generate Vertext Buffer Object for normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    // normal attribute
    glVertexAttribPointer(LIGHTING_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(LIGHTING_ATTRIB_NORMAL);

    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glVertexAttribPointer(CUBE_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(CUBE_ATTRIB_VERTEX);
    // glUniform1i(texture0_, M_TEXTURE0);
    // glUniform1i(texture1_, M_TEXTURE1);
    // glUniform1i(texture2_, M_TEXTURE2);

    return absl::OkStatus();
}

absl::Status Gl14LightingMaterialCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                                      double timestamp) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    int src_width = src.width();
    int src_height = src.height();
    glViewport(0, 0, src_width, src_height);

    // camera
    Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

    lightingShader->use();

    // lighting
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    lightingShader->setVec3("light.position", lightPos);

    lightingShader->setVec3("viewPos", camera.Position);

    // light properties
    glm::vec3 lightColor(1.0f);
    // lightColor.x = static_cast<float>(sin(timestamp * 2.0));
    // lightColor.y = static_cast<float>(sin(timestamp * 0.7));
    // lightColor.z = static_cast<float>(sin(timestamp * 1.3));
    glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
    lightingShader->setVec3("light.ambient", ambientColor);
    lightingShader->setVec3("light.diffuse", diffuseColor);
    lightingShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    lightingShader->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    lightingShader->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    lightingShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
    lightingShader->setFloat("material.shininess", 32.0f);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)src_width / (float)src_height, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    lightingShader->setMat4("projection", projection);
    lightingShader->setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    model = glm::rotate(model, (float) timestamp * glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 1.0f));
    lightingShader->setMat4("model", model);

    // drawring 
    // render the cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // glUseProgram(cubeShader_);
    lightCubeShader->use();
    lightCubeShader->setMat4("projection", projection);
    lightCubeShader->setMat4("view", view);
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    lightCubeShader->setMat4("model", model);

    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, TextureBO[0]);
    // glBindTexture(src.target(), src.name());

    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, TextureBO[0]);

    // glActiveTexture(GL_TEXTURE2);
    // glBindTexture(GL_TEXTURE_2D, TextureBO[1]);
    // glBindTexture(src.target(), src.name());

    // glm::vec3 cubePositions[] = {
    //     glm::vec3( 0.0f,  0.0f,  0.0f), 
    //     glm::vec3( 2.0f,  5.0f, -15.0f), 
    //     glm::vec3(-1.5f, -2.2f, -2.5f),  
    //     glm::vec3(-3.8f, -2.0f, -12.3f),  
    //     glm::vec3( 2.4f, -0.4f, -3.5f),  
    //     glm::vec3(-1.7f,  3.0f, -7.5f),  
    //     glm::vec3( 1.3f, -2.0f, -2.5f),  
    //     glm::vec3( 1.5f,  2.0f, -2.5f), 
    //     glm::vec3( 1.5f,  0.2f, -1.5f), 
    //     glm::vec3(-1.3f,  1.0f, -1.5f)  
    // };
    // glBindVertexArray(VAO);
    // for(unsigned int i = 0; i < 10; i++)
    // {
    //     glm::mat4 model = glm::mat4(1.0f);
    //     model = glm::translate(model, cubePositions[i]);
    //     float angle = 20.0f * i; 
    //     if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.
    //         angle = timestamp * 25.0f;
    //     model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    //     glUniformMatrix4fv(model_, 1, GL_FALSE, glm::value_ptr(model));

    //     glDrawArrays(GL_TRIANGLES, 0, 36);
    // }
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // std::cout << "glDrawArrays texture" << std::endl;
    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // unbind VAO
    // glBindVertexArray(0);

    // unbind textures
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // glActiveTexture(GL_TEXTURE2);
    // glBindTexture(GL_TEXTURE_2D, 0);


    return absl::OkStatus();    
}

absl::Status Gl14LightingMaterialCalculator::GlCleanup() {
    // cleanup
    glDisableVertexAttribArray(LIGHTING_ATTRIB_VERTEX);
    glDisableVertexAttribArray(LIGHTING_ATTRIB_NORMAL);
    glDisableVertexAttribArray(CUBE_ATTRIB_VERTEX);

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(3, VBO);
    // glDeleteBuffers(1, &EBO);

    return absl::OkStatus();    
}

absl::Status Gl14LightingMaterialCalculator::GlTeardown() {
    lightingShader->tearDown();
    lightCubeShader->tearDown();
    return absl::OkStatus();
}

} // mediapipe