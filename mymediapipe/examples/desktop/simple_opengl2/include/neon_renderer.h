/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef NEON_RENDERER_H
#define NEON_RENDERER_H

// #include <glad/glad.h>
#include "common.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "texture.h"
#include "shader.h"
#include "resource_manager.h"

class NeonRenderer
{
public:
    // Render state
    Shader       NeonShader;
    bool         Phase;
    unsigned int Width, Height;
    // Constructor (inits shaders/shapes)
    NeonRenderer(Shader &shader, unsigned int width, unsigned int height);
    // Destructor
    ~NeonRenderer();
    // Renders a defined quad textured with given sprite
    void DrawSprite(Texture2D &texture,
                    glm::vec2 position, 
                    glm::vec2 size = glm::vec2(10.0f, 10.0f), 
                    float rotate = 0.0f, 
                    glm::vec3 color = glm::vec3(1.0f), 
                    float dt = 0.0f);
private:
    unsigned int quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};

#endif