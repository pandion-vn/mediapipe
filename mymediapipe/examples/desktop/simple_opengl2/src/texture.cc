/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <iostream>

#include "../include/texture.h"


Texture2D::Texture2D()
    : Width(0), Height(0), 
      Internal_Format(GL_RGB), Image_Format(GL_RGB), 
      Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), 
      Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
    glGenTextures(1, &this->ID);
}

void Texture2D::Generate(unsigned int width, unsigned int height, const unsigned char* data)
{
    this->Width = width;
    this->Height = height;
    // create Texture
    // std::cout << "Texture2D::Generate glBindTexture ID: " << this->ID << std::endl;
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    // std::cout << "Texture2D::Generate glTexImage2D" << std::endl;
    glGenerateMipmap(GL_TEXTURE_2D);
    // std::cout << "Texture2D::Generate glGenerateMipmap" << std::endl;
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    // unbind texture
    // std::cout << "Texture2D::Generate glBindTexture" << std::endl;
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}

void Texture2D::UnBind() const
{
    glDeleteTextures(1, &this->ID);
}
