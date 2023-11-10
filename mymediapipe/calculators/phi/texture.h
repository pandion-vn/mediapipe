#ifndef TEXTURE_H__
#define TEXTURE_H__

#include "common.h"

enum TextureType{ 
    TextureType_DIFFUSE,
    TextureType_SPECULAR,
    TextureType_NORMAL,
    TextureType_REFLECTION
};

struct Texture {
    GLuint id;
    TextureType m_texture_type;
    std::string m_file_name;
    std::string m_directory;
    std::string m_uniform_name;
    bool  m_has3dTexture;
    
    Texture(std::string file_name, TextureType type, std::string uniform_name = "");
    Texture(std::string directory, std::string uniform_name = "");
    bool Load(std::string directory);
    bool Load3D(std::vector<std::string> textures);
    std::string Get_Uniform_Name(std::string index);
};

#endif
