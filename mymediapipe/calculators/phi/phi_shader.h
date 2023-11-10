#ifndef PHI_SHADER_H__
#define PHI_SHADER_H__

#include "common.h"

class PhiShader {
public:
    GLuint m_program;

private:
    std::vector<std::string> d_vertex_source_path;
    std::vector<std::string> d_fragment_source_path;
    std::vector<std::string> d_geometry_source_path;

    GLchar** d_vertex_code;

    GLchar** d_fragment_code;

    GLchar** d_geometry_code;

    GLint* d_vertex_string_count;
    GLint* d_fragment_string_count;
    GLint* d_geometry_string_count;

    const size_t d_n_vertex;
    const size_t d_n_fragment;
    const size_t d_n_geometry;

    GLint d_mvp_uniform;
    GLint d_vertex_shader;
    GLint d_fragment_shader;
    GLint d_geometry_shader;

public:
    // Constructor reads and builds our shader
    PhiShader(std::string vertexSourcePath, std::string fragmentSourcePath, std::string geometrySourcePath = "");
    PhiShader(std::vector<std::string> vertex_source_paths, std::string fragmentSourcePath, std::string geometrySourcePath = "");
    PhiShader(std::vector<std::string> vertex_source_paths, std::vector<std::string> fragment_source_paths, std::string geometrySourcePath = "");
    ~PhiShader();

    // Use the shader program
    void Use();
    void SetUniform(std::string const &name, glm::vec3 const &value);
    void SetUniform(std::string const &name, glm::vec4 const &value);
    void SetUniform(std::string const &name, glm::mat4 const &value);
    void SetUniform(std::string const &name, float value);
    void SetUniform(std::string const &name, bool value);

private:

    void init();
    void loadVertex();
    void loadFragment();
    void loadGeometry();
    static void load(std::string sourcePath, GLchar*& output, GLint& count);
    void compile();
    void link();
    void initPointers();

    GLint getUniformLocation(std::string const &name) {
        return glGetUniformLocation(m_program, name.c_str());
    }
};

#endif