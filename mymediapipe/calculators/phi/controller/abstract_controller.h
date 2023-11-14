#ifndef ABSTRACT_CONTROLLER_H__
#define ABSTRACT_CONTROLLER_H__

#include "../common.h"
#include "../camera.h"

class AbstractController
{
public:
    explicit AbstractController();
    virtual ~AbstractController();
    virtual void Draw() = 0;
    virtual void Init(/*int argc, char* argv[]*/);
    // virtual void Run() = 0;

protected:
    Camera*     d_camera; //Freed in destructor
    glm::mat4   d_projection_matrix;
    glm::mat4   d_view_matrix;

    glm::vec3   d_light_ambient;
    glm::vec3   d_light_diffuse;
    glm::vec3   d_light_specular;
    glm::vec3   d_light_position;
};

inline AbstractController::AbstractController() :
    d_camera(nullptr),
    d_light_ambient(glm::vec3(0.2f, 0.2f, 0.2f)),
    d_light_diffuse(glm::vec3(0.5f, 0.5f, 0.5f)),
    d_light_specular(glm::vec3(0.5f, 0.5f, 0.5f)) {
    // Empty
}

inline AbstractController::~AbstractController() {
    delete d_camera;
}

inline void AbstractController::Init(/*int argc, char* argv[]*/) {
    this->d_camera = new Camera();
    d_light_position = glm::vec3(-30.0f, 60.0f, 0.0f);
}

#endif