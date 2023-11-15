#ifndef LIGHT_H__
#define LIGHT_H__

#include "../common.h"
#include "../phi_shader.h"

class Light {
public:
    Light(glm::vec3 position, glm::vec3 d_ambient, glm::vec3 d_diffuse, glm::vec3 d_specular);
    void SetShader(PhiShader *shader); 

private:
    glm::vec3 d_position;
    glm::vec3 d_ambient;
    glm::vec3 d_diffuse;
    glm::vec3 d_specular;
};

inline Light::Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    :	d_position(position),
        d_ambient(ambient),
        d_diffuse(diffuse),
        d_specular(specular) { }

inline void Light::SetShader(PhiShader *shader) {
    shader->SetUniform("light.position", d_position);
    shader->SetUniform("light.ambient", d_ambient);
    shader->SetUniform("light.diffuse", d_diffuse);
    shader->SetUniform("light.specular", d_specular);
}

#endif