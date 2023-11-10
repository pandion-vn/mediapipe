#ifndef BONE_H__
#define BONE_H__

#include "common.h"

class AngleRestriction
{
public:
    bool xAxis;
    bool yAxis;
    bool zAxis;

    float xMin;
    float xMax;

    float yMin;
    float yMax;

    float zMin;
    float zMax;

    AngleRestriction(float xMin = -180.0f,float xMax = 180.0f,float yMin = -180.0f,float yMax = 180.0f, float zMin = -180.0f, float zMax = 180.0f) 
        : xMin(glm::radians(xMin)),
        xMax(glm::radians(xMax)),
        yMin(glm::radians(yMin)),
        yMax(glm::radians(yMax)),
        zMin(glm::radians(zMin)),
        zMax(glm::radians(zMax))
    {
        xAxis = true;
        yAxis = true;
        zAxis = true;	
    }

     
};

struct BoneInfo
{
    glm::mat4       offset;
    unsigned int    index;
};

struct Bone {
    Bone() {
        parent = nullptr;
        boneIndex = -1;
    }
    // Reference to its parent
    Bone* parent;

    //Alternatively we can declare a double pointer of children
    //Bone** children;
    std::vector<Bone> children;

    // General index of this bone
    int boneIndex;
    //KeyFrame keyframe;
    std::string name;
    AngleRestriction angleRestriction;
    glm::quat totalRotation;

    //Loaded from Assimp. Transformation from the mesh space to bone space.
    glm::mat4 boneOffset; 
    glm::mat4 transform;
    glm::mat4 globalTransform;
    //Calculated at runtime traversing the tree. offset of the bone in respect of its parent. 
    glm::mat4 finalTransform;
    glm::mat4 localTransform;
    // glm::mat4 getParentTransform();
    glm::mat4 getParentTransform() {
        if (this->parent)
            return parent->globalTransform;
        return glm::mat4(1.0f);
    }

    glm::mat4 getGlobalTransform() {
        return getParentTransform() * glm::inverse(this->boneOffset);
    }

    glm::mat4 getWorldSpace (glm::mat4 const& model) {
        auto position = model * this->finalTransform * glm::inverse(this->boneOffset);  
        return position;
    }

    glm::vec3 getWorldSpacePosition(glm::mat4 const& model) {
        auto position = model * this->finalTransform * glm::inverse(this->boneOffset);  //all this is how i do it
        return decomposeT(position);
    }
};

// inline glm::mat4 Bone::getParentTransform()
// {
//     if (this->parent)
//         return parent->globalTransform;
//     return glm::mat4(1.0f);
// }

#endif