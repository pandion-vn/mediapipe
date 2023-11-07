#include "target.h"

Target::Target(int x, int y, int z) {
    // Add a bit of noise to the target, because if the target
    // starts in a perfect location, the joints might overlap which
    // messes up the algorithm
    position = glm::vec3(x, y, z) + 0.0001f;
    scale = glm::vec3 (.05f, .05f, .05f);
    pitch = 0.0f;
    yaw = 0.0f;

    // Create the shader to use for the controller
    Shader modelS(vertexShaderPath, fragShaderPath);
    objectShader = modelS;

    // Creates the model for the controller
    Model modelM(pathToModel);
    objectModel = modelM;
}

void Target::Render(glm::mat4 view, glm::mat4 proj) {
    objectShader.use();
    
    GLint objectColorLoc = glGetUniformLocation(objectShader.ID, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(objectShader.ID, "lightColor");
    GLint lightPosLoc = glGetUniformLocation(objectShader.ID, "lightPos");
    GLint viewPosLoc = glGetUniformLocation(objectShader.ID, "viewPos");
    
    glUniform3f(objectColorLoc, 1.0f, 1.0f,1.0f);
    glUniform3f(lightColorLoc, 1.0f, 0.0f, 0.0f);
    glUniform3f(lightPosLoc, 1.0f ,1.0f, 3.0f);
    glUniform3f(viewPosLoc,0.0, 0.0, 3.0);
    
    // Calculate the toWorld matrix for the model
    glm::mat4 model;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1, 0, 0));
    R = glm::rotate(R, yaw, glm::vec3(0, 0, 1));
    model = T * R * S;
    
    glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    
    objectModel.Draw(objectShader);
}