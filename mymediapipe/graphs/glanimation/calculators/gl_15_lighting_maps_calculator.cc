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
#include "lib/util.h"

namespace mediapipe {

enum { ATTRIB_VERTEX, ATTRIB_NORMAL, ATTRIB_TEXTURE_COORDS, LIGHTING_NUM_ATTRIBUTES };

// https://learnopengl.com/Lighting/Lighting-maps
class Gl15LightingMapsCalculator : public GlBaseCalculator {
public:
    absl::Status GlSetup() override;
    absl::Status GlBind() override;
    absl::Status GlRender(const GlTexture& src, const GlTexture& dst, double timestamp) override;
    absl::Status GlCleanup() override;
    absl::Status GlTeardown() override;
private:
    GLuint VBO[3], cubeVAO, lightCubeVAO, EBO, TextureBO[2];
    Shader *lightingShader, *lightCubeShader;
    GLuint diffuseMapTexture;
};

REGISTER_CALCULATOR(Gl15LightingMapsCalculator);

absl::Status Gl15LightingMapsCalculator::GlSetup() {
    const GLchar* lighting_vert_src = 
    R"(
        #version 310 es
        precision mediump float;

        in vec3 position;
        in vec3 normal;
        in vec2 texCoords;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 outFragPos;
        out vec3 outNormal;
        out vec2 outTexCoords;

        void main()
        {
            outFragPos = vec3(model * vec4(position, 1.0));
            outNormal = mat3(transpose(inverse(model))) * normal;
            outTexCoords = texCoords;
            
            gl_Position = projection * view * vec4(outFragPos, 1.0);
        }
    )";

    const GLchar* lighting_frag_src = 
    R"(
        #version 310 es
        precision mediump float;

        out vec4 fragColor;

        struct Material {
            sampler2D diffuse;
            // vec3 ambient;
            // vec3 diffuse;
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
        in vec2 outTexCoords;

        // uniform sampler2D texture0;
        uniform vec3 viewPos;
        uniform Material material;
        uniform Light light;

        void main()
        {
            // ambient
            // vec3 ambient = light.ambient * material.ambient;
            vec3 ambient = light.ambient * texture(material.diffuse, outTexCoords).rgb;
            
            // diffuse 
            vec3 norm = normalize(outNormal);
            vec3 lightDir = normalize(light.position - outFragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = light.diffuse * diff * texture(material.diffuse, outTexCoords).rgb;
            
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

    diffuseMapTexture = loadTexture("mymediapipe/assets/opengl/container2.png");

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    std::cout << "DONE setup" << std::endl;
    return absl::OkStatus();
}

absl::Status Gl15LightingMapsCalculator::GlBind() {

    // Initial postition of cube
    GLfloat vertices[] = {
        // positions        
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
        // normals         
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
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
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

    GLfloat texture_coords[] = {
        // texture coords
        0.0f,  0.0f,
        1.0f,  0.0f,
        1.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  1.0f,
        0.0f,  0.0f,

        0.0f,  0.0f,
        1.0f,  0.0f,
        1.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  1.0f,
        0.0f,  0.0f,

        1.0f,  0.0f,
        1.0f,  1.0f,
        0.0f,  1.0f,
        0.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,

        1.0f,  0.0f,
        1.0f,  1.0f,
        0.0f,  1.0f,
        0.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,

        0.0f,  1.0f,
        1.0f,  1.0f,
        1.0f,  0.0f,
        1.0f,  0.0f,
        0.0f,  0.0f,
        0.0f,  1.0f,

        0.0f,  1.0f,
        1.0f,  1.0f,
        1.0f,  0.0f,
        1.0f,  0.0f,
        0.0f,  0.0f,
        0.0f,  1.0f
    };

    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // Generate Vertext Buffer Object for vertex
    glGenBuffers(3, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    // Bind data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // vertex position attribute
    glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    // Generate Vertext Buffer Object for normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    // normal attribute
    glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(ATTRIB_NORMAL);

    // Generate Vertext Buffer Object for normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coords), texture_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(ATTRIB_TEXTURE_COORDS, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(ATTRIB_TEXTURE_COORDS);

    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    return absl::OkStatus();
}

absl::Status Gl15LightingMapsCalculator::GlRender(const GlTexture& src, const GlTexture& dst, 
                                                      double timestamp) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    int src_width = src.width();
    int src_height = src.height();
    glViewport(0, 0, src_width, src_height);

    // camera
    Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));

    lightingShader->use();
    lightingShader->setInt("material.diffuse", 0);

    // lighting
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    lightingShader->setVec3("light.position", lightPos);
    lightingShader->setVec3("viewPos", camera.Position);

    // light properties
    glm::vec3 lightColor;
    // lightColor.x = static_cast<float>(sin(timestamp * 2.0));
    // lightColor.y = static_cast<float>(sin(timestamp * 0.7));
    // lightColor.z = static_cast<float>(sin(timestamp * 1.3));
    // glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
    // glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
    // lightingShader->setVec3("light.ambient", ambientColor);
    // lightingShader->setVec3("light.diffuse", diffuseColor);
    // light properties
    lightingShader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f); 
    lightingShader->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    lightingShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // lightingShader->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    // lightingShader->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    // lightingShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
    lightingShader->setFloat("material.shininess", 32.0f);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)src_width / (float)src_height, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    lightingShader->setMat4("projection", projection);
    lightingShader->setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    lightingShader->setMat4("model", model);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMapTexture);

    // drawring 
    // render the cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

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


    return absl::OkStatus();    
}

absl::Status Gl15LightingMapsCalculator::GlCleanup() {
    // cleanup
    glDisableVertexAttribArray(ATTRIB_VERTEX);
    glDisableVertexAttribArray(ATTRIB_NORMAL);
    glDisableVertexAttribArray(ATTRIB_TEXTURE_COORDS);

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(3, VBO);
    // glDeleteBuffers(1, &EBO);

    return absl::OkStatus();    
}

absl::Status Gl15LightingMapsCalculator::GlTeardown() {
    lightingShader->tearDown();
    lightCubeShader->tearDown();
    return absl::OkStatus();
}

} // mediapipe