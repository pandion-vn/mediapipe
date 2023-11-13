#include "texture.h"

Texture::Texture(std::string file_name, TextureType type, std::string uniform_name) :
    m_texture_type(type),
    m_file_name(file_name), 
    m_uniform_name(uniform_name),
    m_has3dTexture(false) {
    assert(file_name!= "");
}

Texture::Texture(std::string directory, std::string uniform_name /*= ""*/) :
    m_directory(directory),
    m_uniform_name(uniform_name),
    m_has3dTexture(false) {
}

std::string Texture::Get_Uniform_Name(std::string index) {
    if (!m_uniform_name.empty()) return m_uniform_name;

    switch (m_texture_type) {
    case TextureType_REFLECTION:
        return "material.texture_reflection" + index;
        break;
    case TextureType_DIFFUSE:
        return "material.texture_diffuse" + index;
        break;
    case TextureType_SPECULAR:
        return "material.texture_specular" + index;
        break;
    case TextureType_NORMAL:
        return "material.texture_normal" + index;
        break;
    default:
        break;
    }
}

bool Texture::Load(std::string directory)
{
    std::string stringFileName(m_file_name);
    std::string fullPath = directory + "/" + stringFileName;

    int width, height, channels;
    unsigned char *data = stbi_load(fullPath.c_str(), &width, &height, &channels, 0);

    glGenTextures(1, &id);

    if (data) {
        GLenum format;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;

        // Assign texture to ID
        glBindTexture(GL_TEXTURE_2D, id);
        // Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        // glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        std::cout << "Texture failed to load at path: " << fullPath.c_str() << std::endl;
    }
    // delete image; 
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

bool Texture::Load3D(std::vector<std::string> textures)
{
    return true;
    //	GLuint texture;
    //	Magick::Blob blob;
    //	Magick::Image* image; 
    //
    //	if(textures.size() == 0)
    //	{
    //		return false;
    //	}
    //
    //	GLsizei width, height, depth = (GLsizei)textures.size();
    //
    //	string stringFileName(m_file_name);
    //	string fullPath = m_directory + "\\" + textures[0];
    //	try {
    //		image = new Magick::Image(fullPath.c_str());
    //		image->write(&blob, "RGBA");
    //
    //		width = image->columns();
    //		height = image->rows();
    //	}
    //	catch (Magick::Error& Error) {
    //		std::cout << "Error loading texture '" << fullPath << "': " << Error.what() << std::endl;
    //		return false;
    //	}
    //
    //	glGenTextures(1, &id);
    //	glBindTexture(GL_TEXTURE_3D, id);
    //	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //	
    //	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, 
    //		height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    //
    //	glTexSubImage3D(
    //		GL_TEXTURE_3D, 0, 
    //		0, 0, (GLint)1,
    //		width, height, 1,
    //		GL_RGBA, GL_UNSIGNED_BYTE,
    //		blob.data()
    //		);
    //
    ////	delete image;
    //
    //	for (int i = 1; i < textures.size(); i++)
    //	{
    //		fullPath = m_directory + "\\" + textures[i];
    //		image = new Magick::Image(fullPath.c_str());
    //		image->write(&blob, "RGBA");
    //		
    //		glTexSubImage3D(
    //			GL_TEXTURE_3D, 0, 
    //			0, 0, (GLint)i,
    //			width, height, 1,
    //			GL_RGBA, GL_UNSIGNED_BYTE,
    //			blob.data()
    //			);
    //
    //	//	delete image;
    //	} 
    //	m_has3dTexture = true;
    //	glGenerateMipmap(GL_TEXTURE_3D);
    //	glBindTexture(GL_TEXTURE_3D, 0);
}