#ifndef SHADER_H
#define SHADER_H

#include "mediapipe/gpu/shader_util.h"

class Shader
{
public:
    GLuint ID = 0;
    Shader(const GLchar* vert_src, const GLchar* frag_src) {
        mediapipe::GlhCreateProgram(vert_src, frag_src, 0, nullptr, nullptr, &ID);
        // RET_CHECK(ID) << " Problem initializing the program.";
    }

    // activate the shader
    void use() const { 
        glUseProgram(ID); 
    }

    void tearDown() {
        if (ID) {
            glDeleteProgram(ID);
            ID = 0;
        }
    }

    // utility uniform functions
    void setBool(const std::string &name, bool value) const {         
        glUniform1i(UniformLocation(name), (int)value); 
    }

    void setInt(const std::string &name, int value) const { 
        glUniform1i(UniformLocation(name), value); 
    }

    void setFloat(const std::string &name, float value) const { 
        glUniform1f(UniformLocation(name), value); 
    }
    
    void setVec2(const std::string &name, const glm::vec2 &value) const { 
        glUniform2fv(UniformLocation(name), 1, &value[0]); 
    }
    
    void setVec2(const std::string &name, float x, float y) const { 
        glUniform2f(UniformLocation(name), x, y); 
    }

    void setVec3(const std::string &name, const glm::vec3 &value) const { 
        glUniform3fv(UniformLocation(name), 1, &value[0]); 
    }

    void setVec3(const std::string &name, float x, float y, float z) const { 
        glUniform3f(UniformLocation(name), x, y, z); 
    }

    void setVec4(const std::string &name, const glm::vec4 &value) const { 
        glUniform4fv(UniformLocation(name), 1, &value[0]); 
    }

    void setVec4(const std::string &name, float x, float y, float z, float w) const { 
        glUniform4f(UniformLocation(name), x, y, z, w); 
    }
    
    void setMat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(UniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setMat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(UniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(UniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string &name, const std::vector<glm::mat4> &mat) const {
        glUniformMatrix4fv(UniformLocation(name), mat.size(), GL_FALSE, glm::value_ptr(mat[0]));
    }

private:
    GLint UniformLocation(const std::string &name) const {
        return glGetUniformLocation(ID, name.c_str());
    }
};

#endif