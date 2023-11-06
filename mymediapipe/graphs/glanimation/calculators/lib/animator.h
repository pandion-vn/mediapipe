#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <map>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "animation.h"
#include "bone.h"
#include "kalidokit.h"

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
            std::cout << "Calculated all Bone transform" << std::endl;
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f), m_CurrentAnimation, m_CurrentTime);
        }
    }

    void UpdateAnimationWithBone(float dt, 
                                 std::vector<glm::vec3> &landmarks,
                                 std::vector<glm::quat> &rotations) {
        m_DeltaTime = dt;
        if (m_CurrentAnimation) {
            m_CurrentTime = fmod(m_CurrentTime + m_CurrentAnimation->GetTicksPerSecond() * dt, m_CurrentAnimation->GetDuration());
            CalculateBoneTransformWithBone(&m_CurrentAnimation->GetRootNode(), 
                                   glm::mat4(1.0f), 
                                   m_CurrentAnimation, 
                                   m_CurrentTime,
                                   landmarks,
                                   rotations);
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

    glm::mat4 BonePosition(Bone* bone, glm::vec3 position, float scaleFactor=0.3f) {
        KeyPosition pos = bone->GetPositions(0.0);
        // glm::vec3 finalPosition = glm::mix(pos.position, pos.position + position, scaleFactor);
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos.position);

        return translation;
    }

    glm::mat4 BoneRotation(Bone* bone, glm::quat rotation, float scaleFactor=0.3f) {
        KeyRotation rot = bone->GetRotations(0.0);
        // glm::quat finalRotation = glm::slerp(rot.orientation, rotation, scaleFactor);
        glm::quat finalRotation = glm::normalize(rotation);
        // glm::quat finalRotation = rotation;

        return glm::toMat4(finalRotation);
    }

    glm::mat4 BoneScale(Bone* bone, glm::vec3 nextScl, float scaleFactor=0.3f) {
        KeyScale scl = bone->GetScalings(0.0);
        // glm::vec3 finalScale = glm::mix(scl.scale, scl.scale + nextScl, scaleFactor);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), scl.scale);
        return scale;
    }

    void CalculateBoneTransformWithBone(const AssimpNodeData* node, 
                                        glm::mat4 parentTransform, 
                                        Animation* animation, 
                                        float currentTime,
                                        std::vector<glm::vec3> &landmarks,
                                        std::vector<glm::quat> &rotations) {
        std::string nodeName = node->name;
        // std::cout << "CalculateBoneTransform nodeName :" << nodeName << std::endl;
        glm::mat4 nodeTransform = node->transformation;

        Bone* bone = animation->FindBone(nodeName);

        if (bone) {
            // glm::mat4 translate = glm::mat4(1.0f);
            // glm::mat4 scale = glm::mat4(1.0f);
            // glm::mat4 rot = glm::mat4(1.0f);
            // std::string order = "XYZ";
            if (nodeName == "mixamorig_LeftArm") {
                glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
                // glm::quat rot = glm::quat(1.0f, -0.8f, 0.f, 0.f); // up arm
                // glm::quat rot = glm::quat(1.0f, 0.0f, -0.9f, 0.f); // rot up hand from arm
                // glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.9f); // rot to front arm
                // glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // t-pose
                // glm::mat4 rotation = BoneRotation(bone, rot);
                glm::mat4 rotation = BoneRotation(bone, rotations[12]);
                glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
                // std::cout << "rot leftarm" << glm::to_string(rotations[11]) << std::endl;
                nodeTransform = translation * rotation * scale;
            } else if (nodeName == "mixamorig_LeftForeArm") {
                glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
                // glm::quat rot = glm::quat(1.0f, -0.8f, 0.f, 0.f); // up arm
                // glm::quat rot = glm::quat(1.0f, 0.0f, -0.9f, 0.f); // rot up hand from arm
                // glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.9f); // rot to front arm
                // glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // t-pose
                // glm::mat4 rotation = BoneRotation(bone, rot);
                glm::mat4 rotation = BoneRotation(bone, rotations[14]);
                glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
                // std::cout << "rot leftarm" << glm::to_string(rotations[11]) << std::endl;
                nodeTransform = translation * rotation * scale;
            } else if (nodeName == "mixamorig_LeftHand") {
                glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
                // glm::quat rot = glm::quat(1.0f, -0.8f, 0.f, 0.f); // up arm
                // glm::quat rot = glm::quat(1.0f, 0.0f, -0.9f, 0.f); // rot up hand from arm
                // glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.9f); // rot to front arm
                // glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // t-pose
                // glm::mat4 rotation = BoneRotation(bone, rot);
                glm::mat4 rotation = BoneRotation(bone, rotations[16]);
                glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
                // std::cout << "rot leftarm" << glm::to_string(rotations[11]) << std::endl;
                nodeTransform = translation * rotation * scale;
            } else if (nodeName == "mixamorig_RightArm") {
                glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
                // glm::quat rot = glm::quat(1.0f, -0.9f, 0.f, 0.f); // up arm
                // glm::quat rot = glm::quat(1.0f, 0.f, -0.9f, 0.f); // rot up hand from arm
                // glm::quat rot = glm::quat(1.0f, 0.f, 0.f, -0.9f); // to front
                // glm::mat4 rotation = BoneRotation(bone, rot);
                glm::mat4 rotation = BoneRotation(bone, rotations[11]);
                glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
                nodeTransform = translation * rotation * scale;
            } else if (nodeName == "mixamorig_RightForeArm") {
                glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
                // glm::quat rot = glm::quat(1.0f, -0.9f, 0.f, 0.f); // up arm
                // glm::quat rot = glm::quat(1.0f, 0.f, -0.9f, 0.f); // rot up hand from arm
                // glm::quat rot = glm::quat(1.0f, 0.f, 0.f, -0.9f); // to right from shoulder
                // glm::quat rot = glm::quat(1.0f, 0.f, 0.f, 0.f); // T-pose
                // glm::mat4 rotation = BoneRotation(bone, rot);
                glm::mat4 rotation = BoneRotation(bone, rotations[13]);
                glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
                nodeTransform = translation * rotation * scale;
            } else if (nodeName == "mixamorig_RightHand") {
                glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
                // glm::quat rot = glm::quat(1.0f, -0.9f, 0.f, 0.f); // up arm
                // glm::quat rot = glm::quat(1.0f, 0.f, -0.9f, 0.f); // rot up hand from arm
                // glm::quat rot = glm::quat(1.0f, 0.f, 0.f, -1.9f); // to left from forearm
                glm::quat rot = glm::quat(1.0f, 0.f, 0.f, 0.f); // to left from forearm
                glm::mat4 rotation = BoneRotation(bone, rot);
                // glm::mat4 rotation = BoneRotation(bone, rotations[15]);
                glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
                nodeTransform = translation * rotation * scale;
            }

            // if (nodeName == "Head") {
            // } else if (nodeName == "mixamorig_Neck") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0)/*landmarks[34]*/);
            //     glm::mat4 rotation = BoneRotation(bone, rotations[34]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     transform = translation * rotation * scale;
            // } else if (/*nodeName == "Hips" || */nodeName == "mixamorig_Hips") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0)/*landmarks[33]*/);
            //     glm::mat4 rotation = BoneRotation(bone, rotations[33], 0.3);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     transform = translation * rotation * scale;
            // } else if (nodeName == "LeftArm" || nodeName == "mixamorig_LeftArm" ) {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[11]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
                
            //     // float scaleFactor = 0.3f;
            //     // KeyRotation rot = bone->GetRotations(0.0);
            //     // glm::quat rota_ = quatFromEuler(rota[12], order);
            //     // glm::quat finalRotation = glm::slerp(rot.orientation, rota_, scaleFactor);
            //     // finalRotation = glm::normalize(finalRotation);
            //     // glm::mat4 rotation = glm::toMat4(finalRotation);

            //     transform = translation * rotation * scale;
            //     // glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     // // glm::mat4 rotation = BoneRotation(bone, rotations[11]);
            //     // glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
            //     // float scaleFactor = 0.3f;
            //     // KeyRotation rot = bone->GetRotations(0.0);
            //     // glm::quat rota_ = quatFromEuler(rota[11], order);
            //     // glm::quat finalRotation = glm::slerp(rot.orientation, rot.orientation + rota_, scaleFactor);
            //     // finalRotation = glm::normalize(finalRotation);
            //     // glm::mat4 rotation = glm::toMat4(finalRotation);

            //     // transform = translation * rotation * scale;
                
            // } else if (nodeName == "LeftForeArm" || nodeName == "mixamorig_LeftForeArm") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[13]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
            //     // float scaleFactor = 0.3f;
            //     // KeyRotation rot = bone->GetRotations(0.0);
            //     // glm::quat rota_ = quatFromEuler(rota[14], order);
            //     // glm::quat finalRotation = glm::slerp(rot.orientation, rota_, scaleFactor);
            //     // finalRotation = glm::normalize(finalRotation);
            //     // glm::mat4 rotation = glm::toMat4(finalRotation);

            //     transform = translation * rotation * scale;
            //     // glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     // // glm::mat4 rotation = BoneRotation(bone, rotations[13]);
            //     // glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
            //     // float scaleFactor = 0.3f;
            //     // KeyRotation rot = bone->GetRotations(0.0);
            //     // glm::quat rota_ = quatFromEuler(rota[13], order);
            //     // glm::quat finalRotation = glm::slerp(rot.orientation, rot.orientation + rota_, scaleFactor);
            //     // finalRotation = glm::normalize(finalRotation);
            //     // glm::mat4 rotation = glm::toMat4(finalRotation);

            //     // transform = translation * rotation * scale;
                
            // } else if (nodeName == "LeftHand" || nodeName == "mixamorig_LeftHand") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[15]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
            //     // float scaleFactor = 0.3f;
            //     // KeyRotation rot = bone->GetRotations(0.0);
            //     // glm::quat rota_ = quatFromEuler(rota[16], order);
            //     // glm::quat finalRotation = glm::slerp(rot.orientation, rota_, scaleFactor);
            //     // finalRotation = glm::normalize(finalRotation);
            //     // glm::mat4 rotation = glm::toMat4(finalRotation);

            //     transform = translation * rotation * scale;
            //     // glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     // float scaleFactor = 0.3f;
            //     // KeyRotation rot = bone->GetRotations(0.0);
            //     // glm::quat rota_ = quatFromEuler(rota[15], order);
            //     // glm::quat finalRotation = glm::slerp(rot.orientation, rot.orientation + rota_, scaleFactor);
            //     // finalRotation = glm::normalize(finalRotation);
            //     // glm::mat4 rotation = glm::toMat4(finalRotation);

            //     // // glm::mat4 rotation = BoneRotation(bone, rotations[15]);
            //     // glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     // transform = translation * rotation * scale;
            // } else if (nodeName == "RightArm" || nodeName == "mixamorig_RightArm") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     // glm::quat rot = glm::quat(1.0f, -0.03f, -0.9f, .0f);
            //     glm::mat4 rotation = BoneRotation(bone, rotations[12], 1.0);

            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     // std::cout << "left arm rot: " << glm::to_string(bone->GetRotations(0.0).orientation) << std::endl;
            //     // std::cout << "left arm rot static: " << glm::to_string(rot) << std::endl;
            //     // std::cout << "left arm rot calc: " << glm::to_string(rotations[11]) << std::endl;
            //     // float scaleFactor = 0.3f;
            //     // KeyRotation rot = bone->GetRotations(0.0);
            //     // glm::quat rota_ = quatFromEuler(rota[11], order);
            //     // glm::quat finalRotation = glm::slerp(rot.orientation, rota_, scaleFactor);
            //     // finalRotation = glm::normalize(finalRotation);
            //     // glm::mat4 rotation = glm::toMat4(finalRotation);

            //     transform = translation * rotation * scale;
            //     // glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     // glm::mat4 rotation = BoneRotation(bone, rotations[12]);
            //     // glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     // transform = translation * rotation * scale;

            // } else if (nodeName == "RightForeArm" || nodeName == "mixamorig_RightForeArm") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[14]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
            //     // float scaleFactor = 0.3f;
            //     // KeyRotation rot = bone->GetRotations(0.0);
            //     // glm::quat rota_ = quatFromEuler(rota[13], order);
            //     // glm::quat finalRotation = glm::slerp(rot.orientation, rota_, scaleFactor);
            //     // finalRotation = glm::normalize(finalRotation);
            //     // glm::mat4 rotation = glm::toMat4(finalRotation);

            //     transform = translation * rotation * scale;
            //     // glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     // glm::mat4 rotation = BoneRotation(bone, rotations[14]);
            //     // glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     // transform = translation * rotation * scale;
            // } else if (nodeName == "RightHand" || nodeName == "mixamorig_RightHand") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[16]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));
            //     // float scaleFactor = 0.3f;
            //     // KeyRotation rot = bone->GetRotations(0.0);
            //     // glm::quat rota_ = quatFromEuler(rota[15], order);
            //     // glm::quat finalRotation = glm::slerp(rot.orientation, rota_, scaleFactor);
            //     // finalRotation = glm::normalize(finalRotation);
            //     // glm::mat4 rotation = glm::toMat4(finalRotation);

            //     transform = translation * rotation * scale;
            //     // glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     // glm::mat4 rotation = BoneRotation(bone, rotations[16]);
            //     // glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     // transform = translation * rotation * scale;
            // } else if (nodeName == "LeftUpLeg" || nodeName == "mixamorig_LeftUpLeg") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[23]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     transform = translation * rotation * scale;
            // } else if (nodeName == "LeftLeg" || nodeName == "mixamorig_LeftLeg") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[25]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     transform = translation * rotation * scale;
            // } else if (nodeName == "LeftFoot" || nodeName == "mixamorig_LeftFoot") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[27]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     transform = translation * rotation * scale;
            // } else if (nodeName == "RightUpLeg" || nodeName == "mixamorig_RightUpLeg") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[24]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     transform = translation * rotation * scale;
            // } else if (nodeName == "RightLeg" || nodeName == "mixamorig_RightLeg" ) {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[26]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     transform = translation * rotation * scale;
            // } else if (nodeName == "RightFoot" || nodeName == "mixamorig_RightFoot") {
            //     glm::mat4 translation = BonePosition(bone, glm::vec3(0.0));
            //     glm::mat4 rotation = BoneRotation(bone, rotations[28]);
            //     glm::mat4 scale = BoneScale(bone, glm::vec3(0.0));

            //     transform = translation * rotation * scale;
            // }
            
            // nodeTransform = bone->GetLocalTransform();
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
            CalculateBoneTransformWithBone(&node->children[i], globalTransformation, animation, currentTime, landmarks, rotations);
    }

    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, Animation* animation, float currentTime) {
        std::string nodeName = node->name;
        // std::cout << "CalculateBoneTransform nodeName :" << nodeName << std::endl;
        glm::mat4 nodeTransform = node->transformation;

        Bone* bone = animation->FindBone(nodeName);

        if (bone) {
            if (nodeName == "LeftHand") {
                std::cout << "Get transform of lefthand" << std::endl;
            }
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

#endif