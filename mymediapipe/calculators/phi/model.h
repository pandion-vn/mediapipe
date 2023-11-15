#ifndef MODEL_H__
#define MODEL_H__

#include "common.h"
#include "helper.h"
#include "mesh.h"
#include "skeleton.h"
#include "animation/i_animation.h"

class Model 
{
public:
    std::vector<glm::mat4>  m_animation_matrix;
    Skeleton*               m_skeleton;
private: 
    std::vector<Mesh>       d_meshes;
    std::vector<Texture>    d_textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::string             d_directory;
    GLuint*			d_bone_location;
    int				d_numberOfBone;
    glm::mat4		d_model_matrix;
    glm::mat4		d_scale;
    glm::mat4		d_position;
    glm::quat		d_rotation;

public:
    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    explicit Model(std::string path);
    ~Model();
    // Draws the model, and thus all its meshes
    void Draw(PhiShader& shader, bool withAdjacencies = false);
    void LoadModel(std::string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* ai_mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName);

    glm::vec3 Position() const { 
        return decomposeT(d_position); 
    } 

    bool Has_Texture() {
        return d_textures_loaded.size()>0;
    }

    std::vector<Mesh>* GetMeshes() {
        return &d_meshes;
    }

    void Scale(glm::vec3 const& scale_vector) {
        this->d_scale = glm::scale(this->d_scale, scale_vector);
    }

    void Translate(glm::vec3 const& translation_vector) {
        this->d_position = glm::translate(this->d_position, translation_vector);
    }

    void TranslateFromOrigin(glm::vec3 const& translation_vector) {
        this->d_position = glm::translate(glm::mat4(), translation_vector);
    }

    void Rotate(glm::vec3 const& rotation_vector, float radians) {
        this->d_rotation = glm::rotate(d_rotation, radians, rotation_vector);
    }

    glm::quat Rotation() const {
        return d_rotation;
    } 

    void Rotate(glm::quat rotation) {
        this->d_rotation = rotation;
    }

    glm::vec3 GetPositionVec() {
        return decomposeT(d_position);
    }	

    glm::mat4 GetPosition() {
        return d_position;
    }

    glm::mat4 GetModelMatrix() const {
        return d_position * glm::toMat4(d_rotation) * d_scale;
        // return glm::mat4(1.0f);
    }

    void CleanAnimationMatrix() {
        //Initialize bones in the shader for the uniform 
        // for (unsigned int i = 0 ; i < m_skeleton->GetNumberOfBones(); i++) 
        //     m_animation_matrix[i] = glm::mat4(1.0f); 
        for (int i = 0; i < 100; i++)
            m_animation_matrix[i] = glm::mat4(1.0f);
    }

    void Animate(IAnimation* animationInvoker, 
                 glm::vec3 const& target, 
                 std::string const& boneEffector,
                 int numParent = 4) {
        assert(animationInvoker);

        Bone *effector = m_skeleton->GetBone(boneEffector);
        assert(effector);

        animationInvoker->Animate(this->d_model_matrix, effector, target, numParent);

        m_skeleton->UpdateAnimationMatrix(m_animation_matrix);
    }

};
#endif