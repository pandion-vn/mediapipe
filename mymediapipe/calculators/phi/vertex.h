#ifndef VERTEX_H__
#define VERTEX_H__

#define NUM_BONES_PER_VERTEX 4

struct VertexWeight
{
    GLuint IDs[NUM_BONES_PER_VERTEX];
    float Weights[NUM_BONES_PER_VERTEX];
};

struct Vertex {
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
    //BoneData
    VertexWeight WeightData;
    //Color
    glm::vec4 Color;
    //Tangent
    glm::vec3 Tangent;

    Vertex() {}
    Vertex(glm::vec3 position, 
           glm::vec4 color = glm::vec4(1,1,1,1),
           glm::vec3 normal=glm::vec3(0.0f),
           glm::vec3 tangent=glm::vec3(0.0f))
        : Position(position),
          Color(color),
          Normal(normal),
          Tangent(tangent) {}
};

#define TVecCoord std::vector<Vertex>

#endif