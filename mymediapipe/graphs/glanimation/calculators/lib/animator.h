#pragma once

#include <map>
#include <vector>
#include "glm/glm.hpp"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "animation.h"
#include "bone.h"

class Animator
{
public:
    Animator():
        m_CurrentTime(0.0),
        m_Interpolating(false),
        m_HaltTime(0.0),
        m_InterTime(0.0) {
        m_CurrentAnimation = nullptr;
        m_NextAnimation = nullptr;
        m_QueueAnimation = nullptr;

        m_FinalBoneMatrices.reserve(100);

        for (int i = 0; i < 100; i++)
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    void UpdateAnimation(float dt) {
        m_DeltaTime = dt;
        if (m_CurrentAnimation) {
            // m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime + m_CurrentAnimation->GetTicksPerSecond() * dt, m_CurrentAnimation->GetDuration());
            // std::cout << "m_CurrentTime :" << m_CurrentTime << std::endl;

            float transitionTime = m_CurrentAnimation->GetTicksPerSecond() * 0.2f;
            if (m_Interpolating && m_InterTime <= transitionTime) {
                m_InterTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
                CalculateBoneTransition(&m_CurrentAnimation->GetRootNode(), 
                                        glm::mat4(1.0f), 
                                        m_CurrentAnimation, m_NextAnimation, m_HaltTime, m_InterTime, transitionTime);
                return;
            } else if (m_Interpolating) {
                if (m_QueueAnimation) {
                    m_HaltTime = 0.0f;
                    m_NextAnimation = m_QueueAnimation;
                    m_QueueAnimation = nullptr;
                    m_CurrentTime = 0.0f;
                    m_InterTime = 0.0;
                    return;
                }

                m_Interpolating = false;
                m_CurrentAnimation = m_NextAnimation;
                m_CurrentTime = 0.0;
                m_InterTime = 0.0;
            }
            // std::cout << "m_CurrentTime :" << m_CurrentTime << std::endl;
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f), m_CurrentAnimation, m_CurrentTime);
        }
    }

    void PlayAnimation(Animation* pAnimation) {
        if (!m_CurrentAnimation) {
            m_CurrentAnimation = pAnimation;
            return;
        }

        if (m_Interpolating) {
            // Handle interpolating from current interpolation here
            if (pAnimation != m_NextAnimation)
                m_QueueAnimation = pAnimation;
        }
        else {
            // Else: Just playing current animation
            // Start interpolation
            if (pAnimation != m_NextAnimation) {
                m_Interpolating = true;
                m_HaltTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
                m_NextAnimation = pAnimation;
                m_CurrentTime = 0.0f;
                m_InterTime = 0.0;
            }
        }
        // m_CurrentAnimation = pAnimation;
        // m_CurrentTime = 0.0f;
    }

    void CalculateBoneTransition(const AssimpNodeData* curNode, 
                                 glm::mat4 parentTransform, 
                                 Animation* prevAnimation, 
                                 Animation* nextAnimation, 
                                 float haltTime, float currentTime, float transitionTime) {
        std::string nodeName = curNode->name;
        glm::mat4 transform = curNode->transformation;

        Bone* prevBone = prevAnimation->FindBone(nodeName);
        Bone* nextBone = nextAnimation->FindBone(nodeName);

        if (prevBone && nextBone)
        {
            KeyPosition prevPos = prevBone->GetPositions(haltTime);
            KeyRotation prevRot = prevBone->GetRotations(haltTime);
            KeyScale prevScl = prevBone->GetScalings(haltTime);

            KeyPosition nextPos = nextBone->GetPositions(0.0f);
            KeyRotation nextRot = nextBone->GetRotations(0.0f);
            KeyScale nextScl = nextBone->GetScalings(0.0f);

            prevPos.timeStamp = 0.0f;
            prevRot.timeStamp = 0.0f;
            prevScl.timeStamp = 0.0f;

            nextPos.timeStamp = transitionTime;
            nextRot.timeStamp = transitionTime;
            nextScl.timeStamp = transitionTime;

            glm::mat4 p = interpolatePosition(currentTime, prevPos, nextPos);
            glm::mat4 r = interpolateRotation(currentTime, prevRot, nextRot);
            glm::mat4 s = interpolateScaling(currentTime, prevScl, nextScl);

            transform = p * r * s;
        }

        glm::mat4 globalTransformation = parentTransform * transform;

        auto boneInfoMap = nextAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < curNode->childrenCount; i++)
            CalculateBoneTransition(&curNode->children[i], globalTransformation, prevAnimation, nextAnimation, haltTime, currentTime, transitionTime);
    }

    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, Animation* animation, float currentTime) {
        std::string nodeName = node->name;
        // std::cout << "CalculateBoneTransform nodeName :" << nodeName << std::endl;
        glm::mat4 nodeTransform = node->transformation;

        Bone* bone = animation->FindBone(nodeName);

        if (bone) {
            bone->Update(currentTime);
            nodeTransform = bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = animation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
            // std::cout << "Update m_FinalBoneMatrices index :" << index << std::endl;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation, animation, currentTime);
    }

    std::vector<glm::mat4> GetFinalBoneMatrices() {
        return m_FinalBoneMatrices;
    }

private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    Animation* m_NextAnimation;
    Animation* m_QueueAnimation;
    float 	m_CurrentTime;
    float 	m_DeltaTime;
    bool 	m_Interpolating;
    float 	m_HaltTime;
    float 	m_InterTime;

    float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    glm::mat4 interpolatePosition(float animationTime, KeyPosition from, KeyPosition to)
    {
        float scaleFactor = getScaleFactor(from.timeStamp, to.timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(from.position, to.position, scaleFactor);
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), finalPosition);
        return translation;
    }

    glm::mat4 interpolateRotation(float animationTime, KeyRotation from, KeyRotation to)
    {
        float scaleFactor = getScaleFactor(from.timeStamp, to.timeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(from.orientation, to.orientation, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }

    glm::mat4 interpolateScaling(float animationTime, KeyScale from, KeyScale to)
    {
        float scaleFactor = getScaleFactor(from.timeStamp, to.timeStamp, animationTime);
        glm::vec3 finalScale = glm::mix(from.scale, to.scale, scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }

};