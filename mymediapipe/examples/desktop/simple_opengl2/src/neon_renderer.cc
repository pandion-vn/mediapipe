/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "../include/neon_renderer.h"


NeonRenderer::NeonRenderer(Shader &shader,unsigned int width, unsigned int height):NeonShader(shader),Width(width),Height(height),Phase(0)
{
    // this->shader = shader;
    this->initRenderData();
}

NeonRenderer::~NeonRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void NeonRenderer::DrawSprite(Texture2D &texture, 
                              glm::vec2 position, 
                              glm::vec2 size, 
                              float rotate, 
                              glm::vec3 color, 
                              float dt)
{
    // prepare transformations
    this->NeonShader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    this->NeonShader.SetMatrix4("model", model);

    // render textured quad
    this->NeonShader.SetVector3f("spriteColor", color);

    this->NeonShader.SetInteger("phase", this->Phase);
    // render time
    this->NeonShader.SetFloat("time", dt);
    // this->NeonShader.SetInteger("phase", phase);
    this->NeonShader.SetVector2f("resolution", glm::vec2(this->Width, this->Height));

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    //glBindVertexArray(this->quadVAO);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    //glBindVertexArray(0);

    Texture2D viewfinder = ResourceManager::GetTexture("viewfinder");
    glActiveTexture(GL_TEXTURE1);
    viewfinder.Bind();

    Texture2D background = ResourceManager::GetTexture("background");
    glActiveTexture(GL_TEXTURE2);
    background.Bind();
    
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void NeonRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
