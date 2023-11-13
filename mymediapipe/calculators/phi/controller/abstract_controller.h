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
};

inline AbstractController::AbstractController() :
    d_camera(nullptr)
    {
    // Empty
}

inline AbstractController::~AbstractController() {
    delete d_camera;
}

inline void AbstractController::Init(/*int argc, char* argv[]*/) {
    this->d_camera = new Camera();
}

#endif