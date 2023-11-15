#ifndef IANIMATION_H__
#define IANIMATION_H__

#include "../common.h"
#include "../bone.h"
#include "../skeleton.h"

class IAnimation
{
protected:
    IAnimation(Skeleton* skeleton, glm::mat4 model)
        : skeleton(skeleton), model(model) { }
public:
    //Pure virtual functions
    virtual void Animate(glm::mat4 model, Bone* bone, glm::vec3 target = glm::vec3(), int numParents = 4) = 0;
    virtual void Animate(glm::mat4 model, float animationTime, glm::mat4* animationSequence) = 0;

protected:
    Skeleton* skeleton;
    glm::mat4 model;
};

#endif