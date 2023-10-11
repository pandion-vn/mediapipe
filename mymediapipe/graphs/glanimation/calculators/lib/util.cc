#include <iostream>
#include "util.h"

GLuint loadTexture(char const *path, bool gamma) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    // stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

GLuint TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    std::cout << "Texture file: " << filename << std::endl;

    return loadTexture(filename.c_str(), gamma);

    // unsigned int textureID;
    // glGenTextures(1, &textureID);

    // int width, height, nrComponents;
    // // stbi_set_flip_vertically_on_load(true);
    // unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    // if (data) {
    //     GLenum format;
    //     if (nrComponents == 1)
    //         format = GL_RED;
    //     else if (nrComponents == 3)
    //         format = GL_RGB;
    //     else if (nrComponents == 4)
    //         format = GL_RGBA;

    //     std::cout << "size width: " << width << " height: " << height << " channels: " << nrComponents << std::endl;

    //     glBindTexture(GL_TEXTURE_2D, textureID);
    //     // glTexParameteri(GL_TEXTURE_2D, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
    //     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //     glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);

    //     // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    //     glBindTexture(GL_TEXTURE_2D, 0);
    //     stbi_image_free(data);
    // }
    // else
    // {
    //     std::cout << "Texture failed to load at path: " << path << std::endl;
    //     stbi_image_free(data);
    // }

    // return textureID;
}

GLuint TextureFromEmbedded(const aiTexture* paiTexture) {
    std::cout << "Embeddeded diffuse texture type: " << paiTexture->achFormatHint << std::endl;
    int bufferSize  = paiTexture->mWidth;
    void* pData     = paiTexture->pcData;
    int width, height, nrComponents;
    void* image_data = stbi_load_from_memory((const stbi_uc*)pData, bufferSize, &width, &height, &nrComponents, 0);
    GLuint textureID;
    glGenTextures(1, &textureID);
    if (image_data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        std::cout << "size width: " << width << " height: " << height << " channels: " << nrComponents << std::endl;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        std::cout << "Texture failed to load embedded data" << std::endl;
    }

    stbi_image_free(image_data);
    return textureID;
}

glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
    glm::mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

glm::vec3 GetGLMVec3(const aiVector3D& vec) { 
    return glm::vec3(vec.x, vec.y, vec.z); 
}

glm::vec2 GetGLMVec2(const aiVector3D& vec) { 
    return glm::vec2(vec.x, vec.y); 
}

glm::quat GetGLMQuat(const aiQuaternion& pOrientation) {
    return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}