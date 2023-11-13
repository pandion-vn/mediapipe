#ifndef MESH_H__
#define MESH_H__

#include "common.h"
#include "bone.h"
#include "vertex.h"
#include "texture.h"
// #include "phi_shader.h"
#include "material.h"

class Mesh {
public:
    /*  Mesh Data  */
    //vector<Vertex>            m_vertices;
    TVecCoord                   m_vertices;
    TVecCoord                   m_normals;
    std::vector<glm::vec2>              m_texCoords;
    std::vector<GLuint>                 m_indices;
    std::vector<GLuint>                 m_adjacent_indices;
    std::vector<Texture>                m_textures; 
    std::vector<VertexWeight>           m_boneWeights;
    glm::vec3                           m_center_of_mass; 
    glm::vec3                           m_polyhedral_center_of_mass;

private:
    //BoundingBox                       d_bounding_box;
    //BoundingSphere                    d_bounding_sphere;
    /*  Render data  */
    GLuint                          d_VAO;
    GLuint                          d_VBO;
    GLuint                          d_VBO_textures;
    GLuint                          d_VBO_normals;
    GLuint                          d_EBO;
    GLuint                          d_bone_VBO;
    std::map<std::string, Bone>     d_bone_mapping;
    Material                        d_material;
    float                           d_area;

public:
    Mesh(TVecCoord vertices, 
         std::vector<GLuint> indices, 
         std::vector<Texture> textures, 
         std::vector<VertexWeight> boneWeights, 
         std::vector<GLuint> adjacent_indices, 
         Material material, 
         std::vector<glm::vec2> textCoords, 
         TVecCoord normals);
    void Draw(PhiShader& shader, bool withAdjecencies = false);
    float Area() const { return d_area; } 

private:
    bool hasBones() const {
        return m_boneWeights.size() > 0;
    }
    /*  Functions    */
    // Initializes all the buffer objects/arrays
    void setupMesh();

    // Calculation of the center of mass based on paul bourke's website
    // http://paulbourke.net/geometry/polygonmesh/
    void calculateCenterOfMass();
    void calculateArea();
    void calculateBoundingBox();
    void calculateTexCoord();
};

inline Mesh::Mesh(TVecCoord vertices, 
                  std::vector<GLuint> indices, 
                  std::vector<Texture> textures, 
                  std::vector<VertexWeight> boneWeights, 
                  std::vector<GLuint> adjacent_indices, 
                  Material material, 
                  std::vector<glm::vec2> textCoords, 
                  TVecCoord normals): 
    m_adjacent_indices(adjacent_indices),
    m_normals(normals),
    //d_bounding_box(BoundingBox(nullptr)),
    //d_bounding_sphere(BoundingSphere(NULL)),
    d_material(material),
    d_area(0.0f) { 
    this->m_vertices = vertices;
    this->m_indices = indices;
    this->m_textures = textures; 
    this->m_boneWeights = boneWeights; 
    this->m_texCoords = textCoords;
    //this->calculate_center_of_mass();
    //this->calculateArea();
    //this->calculateBoundingBox(); 
    //this->calculate_tex_coord();
    this->setupMesh();
}

inline void Mesh::setupMesh() {
    // Create buffers/arrays
    glGenVertexArrays(1, &this->d_VAO);

    glGenBuffers(1, &this->d_VBO);
    glGenBuffers(1, &this->d_EBO);
    // glGenBuffers(1, &this->d_VBO_textures);
    // glGenBuffers(1, &this->d_VBO_normals);

    glBindVertexArray(this->d_VAO);
    // Load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, this->d_VBO);
    // A great thing about struct is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(TCoord), &this->m_vertices[0], GL_STATIC_DRAW); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->d_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_indices.size() * sizeof(GLuint), &this->m_indices[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TCoord), (GLvoid*)0);
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, this->d_VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, this->m_normals.size() * sizeof(TCoord), &this->m_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TCoord), (GLvoid*)0);
    // Vertex Texture Coordinates
    // glEnableVertexAttribArray(2);
    // glBindBuffer(GL_ARRAY_BUFFER, this->d_VBO_textures);
    // glBufferData(GL_ARRAY_BUFFER, this->m_texCoords.size() * sizeof(glm::vec2), &this->m_texCoords[0], GL_STATIC_DRAW); 
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    /*
    glEnableVertexAttribArray(3);	
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
    */

    //
    // glEnableVertexAttribArray(4);	
    // glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));

    // if (hasBones())
    // {
    //     glGenBuffers(1, &this->d_bone_VBO);

    //     glBindBuffer(GL_ARRAY_BUFFER, d_bone_VBO);
    //     glBufferData(GL_ARRAY_BUFFER, sizeof(m_boneWeights[0]) * m_boneWeights.size(), &m_boneWeights[0], GL_STATIC_DRAW);

    //     glEnableVertexAttribArray(5);
    //     glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexWeight), (const GLvoid*)0);

    //     glEnableVertexAttribArray(6);    
    //     glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexWeight), (const GLvoid*)offsetof(VertexWeight, Weights)); 
    // }

    glBindVertexArray(0);
}

inline void Mesh::Draw(PhiShader& shader, bool withAdjecencies) {
    shader.Use();
    // if (this->m_textures.size()>0) {
    //     for(GLuint i = 0; i < this->m_textures.size(); i++) {
    //         GLuint textureType = GL_TEXTURE_2D;
    //         glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
    //         // Retrieve texture number (the N in diffuse_textureN)
    //         std::stringstream ss;
    //         std::string number;
    //         auto uniform_name = m_textures[i].Get_Uniform_Name("1");
    //         //if(name == "material.texture_diffuse")
    //         //	ss << diffuseNr++; // Transfer GLuint to stream
    //         //else if(name == "material.texture_specular")
    //         //	ss << specularNr++; // Transfer GLuint to stream
    //         //number = ss.str(); 
    //         // Now set the sampler to the correct texture unit
    //         GLuint shader_location = glGetUniformLocation(shader.m_program,  uniform_name.c_str());
    //         glUniform1i(shader_location, i);
    //         // And finally bind the texture
    //         glBindTexture(textureType, this->m_textures[i].id);
    //     }
    //     glActiveTexture(GL_TEXTURE0); // Always good practice to set everything back to defaults once configured.
    // }
    // std::cout << "Set texture" << std::endl;

    // d_material.SetShader(shader);
    // std::cout << "Set material" << std::endl;

    // Draw mesh
    glBindVertexArray(this->d_VAO);

    GLuint drawMode = withAdjecencies ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;
    GLuint indices_size = withAdjecencies ? m_adjacent_indices.size() : m_indices.size();
    // if (withAdjecencies) {
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->d_EBO);
    //     glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_adjacent_indices.size() * sizeof(GLuint), &this->m_adjacent_indices[0], GL_STATIC_DRAW);
    // }
    glDrawElements(drawMode, indices_size, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

#endif