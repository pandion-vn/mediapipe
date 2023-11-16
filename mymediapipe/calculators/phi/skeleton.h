#ifndef SKELETON_H
#define SKELETON_H

#include "common.h"
#include "bone.h"

class Skeleton 
{
private:
    int d_num_of_bones; 

public:
    // Root node of the tree
    Bone* m_root_bone;  
    // name - Bone Offset mapping. Used to load, during a first loading step, information about the bone structure in Assimp.
    std::map<std::string, BoneInfo> boneMapping;
    glm::mat4 m_inverse_global;

    Skeleton() {
        m_root_bone = new Bone();
        d_num_of_bones = -1;//Initial Value
    }

    ~Skeleton(){
        delete m_root_bone;
        // TODO
        //Consider a recursive method to free all the space
    }

    void UpdateSkeleton(Bone* bone = nullptr) {
        if (d_num_of_bones == 0) return;

        RootBoneCheck(&bone);

        bone->globalTransform =  bone->GetParentTransform() * bone->transform * bone->localTransform;
        bone->finalTransform = m_inverse_global * bone->globalTransform  * bone->boneOffset;  
        // if (true) { // debug
        //     std::cout << "bone: " << bone->name << std::endl
        //               << "  parent transform: " << glm::to_string(bone->GetParentTransform()) << std::endl
        //               << "  transform: " << glm::to_string(bone->transform) << std::endl
        //               << "  boneOffset: " << glm::to_string(bone->boneOffset) << std::endl
        //               << "  globalTransform: " << glm::to_string(bone->globalTransform) << std::endl
        //               << "  localTransform: " << glm::to_string(bone->localTransform) << std::endl
        //               << "  finalTransform: " << glm::to_string(bone->finalTransform) 
        //               << std::endl;
        // }

        for (int i = 0; i < bone->children.size(); i++) {
            UpdateSkeleton(&bone->children[i]);
        }
    }

    void TraversePositions(Bone* bone, glm::mat4 const& modelMatrix, std::vector<glm::vec3> &positions) {
        RootBoneCheck(&bone);

        positions.push_back(bone->GetWorldSpacePosition(modelMatrix));

        for (int i = 0; i < bone->children.size(); i++) {
            TraversePositions(&bone->children[i],modelMatrix,positions);
        }
    }

    std::vector<glm::vec3> GetBonePositions(glm::mat4 const& modelMatrix) {
        std::vector<glm::vec3> positions;

        TraversePositions(m_root_bone, modelMatrix, positions);

        return positions;
    } 

    glm::vec3 GetBonePosition(std::string boneName, glm::mat4 const& modelMatrix) { 
        Bone* bone = GetBone(boneName);

        if (bone)
            return bone->GetWorldSpacePosition(modelMatrix); 

        std::cout << "Not found bone!" << std::endl;
        return glm::vec3();
    } 

    // Import the hierarchical data structure from Assimp
    bool ImportSkeletonBone(aiNode* assimp_node, Bone* bone = nullptr) {  
        bool has_bone = false;
        bool has_useful_child = false;
        std::string boneName = assimp_node->mName.C_Str();
        // std::cout << "Bone: " << boneName << std::endl;

        RootBoneCheck(&bone);

        bone->children.resize((int)assimp_node->mNumChildren);

        if (this->boneMapping.find(boneName) != this->boneMapping.end())  {
            // strcpy_s (bone->name, boneName);
            bone->name = boneName;
            has_bone = true;
        }

        for (int i = 0; i < (int)assimp_node->mNumChildren; i++)  { 
            //Bone* recursiveBone = bone;
            ////I'm setting the parent here
            //if (has_bone) {
            bone->children[i].parent = bone; 
            //}

            bool importRes = ImportSkeletonBone(assimp_node->mChildren[i], &bone->children[i]);
            if (importRes) 
                has_useful_child = true; 
        }

        if (has_useful_child || has_bone) {
            // std::string nodeName(boneName);
            // std::string globalTransf("Node Name " + nodeName + "\n Global Transform");

            bone->boneOffset = this->boneMapping[bone->name].offset;
            bone->boneIndex = this->boneMapping[bone->name].index;

            if (bone->name == boneName)
                bone->transform = aiMatrix4x4ToGlm(&assimp_node->mTransformation);

            return true;
        }
        // no bone or good children - cull self
        //free(bone);
        //bone = NULL;
        return false;
    }

    Bone* GetBone(int boneIndex, Bone* boneToFind = nullptr) {
        RootBoneCheck(&boneToFind);

        // look for match
        if ( boneToFind->boneIndex == boneIndex) {
            return boneToFind;
        }

        // recurse to children
        for (int i = 0; i < boneToFind->children.size(); i++) {
            auto child = GetBone(boneIndex, &boneToFind->children[i]);
            if (child != nullptr) {
                return child;
            }
        }

        // no children match and no self match
        return nullptr;
    }

    // Retrieve a bone given the name
    Bone* GetBone(const std::string node_name, Bone* boneToFind = nullptr) {
        RootBoneCheck(&boneToFind); 

        // look for match
        if (node_name == boneToFind->name) {
            return boneToFind;
        }

        // recurse to children
        for (unsigned int i = 0; i < boneToFind->children.size(); i++) {
            auto child = GetBone(node_name, &boneToFind->children[i]);
            if (child != nullptr) {
                return child;
            }
        }

        // no children match and no self match
        return nullptr;
    }


    // get the total number of bones. traverses the tree to count them
    int GetNumberOfBones() { 
        if (d_num_of_bones != -1) return d_num_of_bones;

        d_num_of_bones = TraverseGetNumberOfBones(m_root_bone);

        return d_num_of_bones;
    }

    void UpdateAnimationMatrix(std::vector<glm::mat4> &animationMatrix, Bone* bone = nullptr) {
        assert(animationMatrix);

        RootBoneCheck(&bone); 

        if (bone->boneIndex >= 0)
            animationMatrix[bone->boneIndex] = bone->finalTransform;

        for (unsigned int i = 0; i < bone->children.size(); i++) {
            UpdateAnimationMatrix(animationMatrix, &bone->children[i]);
        }
    } 

    void ResetAllJointLimits(Bone* bone = nullptr) {
        RootBoneCheck(&bone);
        AngleRestriction newRestr;
        bone->angleRestriction = newRestr;	

        for (int i = 0; i < bone->children.size(); i++)
            ResetAllJointLimits(&bone->children[i]);
    }

private:
    void RootBoneCheck(Bone** bone) {
        if(!*bone)
            *bone = m_root_bone;

        assert(*bone);
    }

    int TraverseGetNumberOfBones(Bone* bone) {
        RootBoneCheck(&bone);

        int counter = bone->boneIndex > -1 ? 1 : 0;

        for (unsigned int i = 0; i < bone->children.size(); i++)
            counter += TraverseGetNumberOfBones(&bone->children[i]);

        return counter;
    }
};

#endif
