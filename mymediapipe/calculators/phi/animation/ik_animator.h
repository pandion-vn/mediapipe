#ifndef IKSOLVER_H__
#define IKSOLVER_H__

#include "../common.h"
#include "../bone.h"
#include "../skeleton.h"
#include "i_animation.h"

#define RADIANS_180 glm::radians(180.0f)
#define RADIANS_360 glm::radians(360.0f)

struct IKInfo
{
    float cosAngle;
    float distance;
    float degreeAngle;
    float crossProductAngle;
    float boneSpaceCrossProductAngle;
    glm::vec3 crossProduct;
    glm::vec3 currentWorldPosition;
    glm::vec3 effectorWorldPosition;
    int iteration;
    std::string currBoneName;
    glm::vec3 boneSpaceCrossProduct;
    IKInfo() {
        iteration	= 0;
        cosAngle	= 0;
        degreeAngle = 0;
        distance	= 0;
    }
};

class IKAnimator : public IAnimation {
public:
    IKAnimator(Skeleton *skeleton) 
        : IAnimation(skeleton, glm::mat4(1)) {
        assert(skeleton);
        maxNumIterations = 100;
        distanceThreshold = 0.01f;
    }

private: 
    int maxNumIterations;
    float distanceThreshold;

public:
    IKInfo ComputeOneCCDLink(const glm::mat4 model, 
                             const glm::vec3 targetWorldPosition,
                             glm::mat4* animations,
                             glm::mat4* animationMatrixes,
                             const char* effectorName,
                             bool simulate,
                             bool reset = false) {
        glm::vec3           currBoneWorldPosition, effectorWorldPosition, targetWorldPositionNormVector, effectorCurrBoneNormVector;
        glm::vec3           crossProduct, boneSpaceCrossProduct;
        IKInfo              ik;
        Bone*               currBone;

        Bone* effectorBone = skeleton->GetBone(effectorName);

        static  Bone*       currentIterationBone = effectorBone;
        static int          currentIteration = 0;

        float               cosAngle = 0,turnAngle = 0;
        float               turnDeg = 0;
        float               distance = 0; 

        if (reset) { 
            currentIterationBone = effectorBone;
            currentIteration = 0;
        }

        currBone                = currentIterationBone->parent;
        effectorWorldPosition   = effectorBone->GetWorldSpacePosition(model);
        distance                = glm::distance(effectorWorldPosition, targetWorldPosition);
        currBoneWorldPosition   = currBone->GetWorldSpacePosition(model);
        glm::vec3 test          = effectorWorldPosition - currBoneWorldPosition;

        effectorCurrBoneNormVector      = glm::normalize(effectorWorldPosition - currBoneWorldPosition);
        targetWorldPositionNormVector   = glm::normalize(targetWorldPosition - currBoneWorldPosition); 

        if ((effectorCurrBoneNormVector != effectorCurrBoneNormVector) || (targetWorldPositionNormVector != targetWorldPositionNormVector))
            goto EXIT;

        cosAngle =  glm::dot( targetWorldPositionNormVector, effectorCurrBoneNormVector);

        if (cosAngle >= 1)
            goto EXIT;

        distance = glm::distance(effectorWorldPosition, targetWorldPosition);

        if (distance < distanceThreshold)
            goto EXIT;

        crossProduct = glm::normalize(glm::cross(effectorCurrBoneNormVector, targetWorldPositionNormVector));
        crossProduct = glm::mat3(glm::inverse(model * currBone->finalTransform * glm::inverse(currBone->boneOffset))) * crossProduct; //this looks weird


        ik.crossProductAngle = glm::degrees(glm::acos(glm::dot(crossProduct,targetWorldPositionNormVector))); 
        ik.boneSpaceCrossProductAngle = glm::degrees(glm::acos(glm::dot(boneSpaceCrossProduct,targetWorldPositionNormVector)));

        if ((crossProduct != glm::vec3()) && crossProduct == crossProduct) { 
            glm::quat quatRotation = glm::angleAxis(glm::acos(cosAngle),  glm::normalize(crossProduct));
            glm::mat4 quatRotationMatrix = glm::toMat4(glm::normalize(quatRotation)); /* */

            if (!simulate) {			 
                currBone->transform  *=   quatRotationMatrix;
                skeleton->UpdateSkeleton(currBone);
            }
        }

EXIT:
        //Update the world position of the effector...
        //effectorWorldPosition  =  effectorBone->getWorldSpacePosition(model);
        if (!simulate)
            currentIteration++;

        ik.crossProduct = crossProduct;
        ik.boneSpaceCrossProduct = boneSpaceCrossProduct;
        ik.cosAngle = cosAngle; 
        ik.iteration = currentIteration;
        ik.degreeAngle = turnDeg;
        ik.effectorWorldPosition = effectorCurrBoneNormVector;
        ik.currentWorldPosition = targetWorldPositionNormVector;
        ik.distance = distance;
        ik.currBoneName = currBone->name;

        if (!simulate)
            if (currBone->boneIndex > 0)
                currentIterationBone = currBone; 
            else
                //Start again from the bone above the parent
                currentIterationBone = effectorBone;

        return ik;
    }

    void SetMaxNumIterations(int maxIterations) {
        this->maxNumIterations = maxNumIterations;
    }

    void SetDistanceThreshold(float distanceThreshold) {
        this->distanceThreshold = distanceThreshold;
    }

    virtual void Animate(glm::mat4 model, Bone* bone, glm::vec3 targetPosition, int numParents = 6) {
        glm::vec3		currBoneWorldPosition, effectorPosition, targetWorldPositionNormVector, effectorCurrBoneNormVector;
        glm::vec3		crossProduct;
        float			cosAngle,turnAngle;
        float			turnDeg, distance; 
        int				currentIteration = 0;
        int				numOfBones = skeleton->GetNumberOfBones();

        Bone* effectorBone = bone;
        Bone* currBone = effectorBone->parent;

        effectorPosition  =  effectorBone->GetWorldSpacePosition(model);

        distance = glm::distance(effectorPosition, targetPosition);
        std::cout << "Animate distance : " << distance << std::endl;

        while (currentIteration++ < maxNumIterations &&  distance > distanceThreshold) {
            currBoneWorldPosition = currBone->GetWorldSpacePosition(model);
            effectorCurrBoneNormVector    = glm::normalize(effectorPosition - currBoneWorldPosition);
            targetWorldPositionNormVector = glm::normalize(targetPosition - currBoneWorldPosition); 

            if ((effectorCurrBoneNormVector != effectorCurrBoneNormVector) || 
                (targetWorldPositionNormVector != targetWorldPositionNormVector))
                break;

            cosAngle = glm::dot(targetWorldPositionNormVector, effectorCurrBoneNormVector); 

            if (cosAngle >= 1)
                break;

            distance = glm::distance(effectorPosition, targetPosition);

            if (distance < distanceThreshold)
                break;


            crossProduct = glm::normalize(glm::cross(effectorCurrBoneNormVector, targetWorldPositionNormVector));
            crossProduct = glm::mat3(glm::inverse(currBone->GetWorldSpace(model))) * crossProduct;

            if ((crossProduct != glm::vec3()) && crossProduct == crossProduct) {
                glm::quat quatRotation = glm::angleAxis(glm::acos(cosAngle), glm::normalize(crossProduct)); //I'm using the radians not the degrees
                currBone->totalRotation *= quatRotation;
                CheckAngleRestrictions(currBone);
                glm::mat4 quatRotationMatrix = glm::toMat4(currBone->totalRotation); 
                currBone->localTransform = quatRotationMatrix;
                skeleton->UpdateSkeleton(currBone);
                // std::cout << "Update bone position!" << currBone->name << std::endl;
            }
            else
            {
                std::cout << "Singularity!" << std::endl;
                //throw new std::exception("Singularity");
            }

            //Update the world position of the effector...
            effectorPosition  =  effectorBone->GetWorldSpacePosition(model);

            if (currBone->boneIndex > 0 && numParents > 0 && currentIteration % numParents != 0) // Second condition is for limit the number of parents in the skeleton to recurse upon
                currBone = currBone->parent; 
            else {
                //Start again from the bone above the parent
                currBone = effectorBone->parent;
            }
        }
    }

    virtual void Animate(glm::mat4 model, float animationTime, glm::mat4* animationSequence) {
        // throw new exception("not implemented");
    }

private:
    void CheckAngleRestrictions(Bone* bone) { 
        glm::vec3 euler = glm::eulerAngles(bone->totalRotation);

        if(bone->angleRestriction.xAxis) {
            if(euler.x > RADIANS_180)
                euler.x -= RADIANS_360;
            euler.x = glm::clamp(euler.x, bone->angleRestriction.xMin, bone->angleRestriction.xMax);
        }

        if(bone->angleRestriction.yAxis) {
            if(euler.y > RADIANS_180)
                euler.y -= RADIANS_360;
            euler.y = glm::clamp(euler.y, bone->angleRestriction.yMin, bone->angleRestriction.yMax);
        }

        if(bone->angleRestriction.zAxis) {
            if(euler.z > RADIANS_180)
                euler.z -= RADIANS_360;
            euler.z = glm::clamp(euler.z, bone->angleRestriction.zMin, bone->angleRestriction.zMax);
        }

        bone->totalRotation = glm::quat(euler); 
    }

};

#endif // IKSOLVER_H__