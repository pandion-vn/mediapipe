#include "model.h"

/*  Functions   */
Model::Model(std::string path): 
    d_numberOfBone(0),
    d_model_matrix(1.0f),
    d_scale(1.0f),
    d_position(1.0f),
    d_rotation(1.0f, 0.0f, 0.0f, 0.0f) {
    assert(path);
    m_skeleton = new Skeleton();
    this->LoadModel(path); 
    if (m_skeleton->GetNumberOfBones() > 0) {
        m_animation_matrix.reserve(100);
        for (int i = 0; i < 100; i++)
            m_animation_matrix.push_back(glm::mat4(1.0f));
        // m_animation_matrix = (glm::mat4*) malloc(m_skeleton->GetNumberOfBones() * sizeof(glm::mat4));

        this->CleanAnimationMatrix();
        m_skeleton->UpdateSkeleton();
        m_skeleton->UpdateAnimationMatrix(m_animation_matrix); 
    }
}

Model::~Model() {
    if (d_bone_location)
        delete d_bone_location;
    delete m_skeleton;
}

void Model::Draw(PhiShader& shader, bool withAdjacencies) {
    shader.Use();

    this->d_model_matrix = GetModelMatrix();
    if (m_skeleton->GetNumberOfBones() > 0) {
        for (int i = 0; i < m_animation_matrix.size(); ++i) {
            shader.SetUniform("bones[" + std::to_string(i) + "]", m_animation_matrix[i]);
            // std::cout << "m_animation_matrix[" << i << "]" << glm::to_string(m_animation_matrix[i]) << std::endl;
        }
    }
    //     glUniformMatrix4fv(d_bone_location[0], m_skeleton->GetNumberOfBones(), GL_FALSE, glm::value_ptr(m_animation_matrix[0]));

    // shader.SetUniform("hasTexture", Has_Texture());
    // std::cout << "Shader set hasTexture" << std::endl;

    for (GLuint i = 0; i < this->d_meshes.size(); i++)
        this->d_meshes[i].Draw(shader, withAdjacencies);
}

// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::LoadModel(std::string path) {
    // Read file via ASSIMP
    Assimp::Importer importer; 
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | 
                                                   aiProcess_GenNormals | 
                                                   aiProcess_FlipUVs |
                                                   aiProcess_GenUVCoords |
                                                   aiProcess_JoinIdenticalVertices);
    // Check for errors
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { // if is Not Zero
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Retrieve the directory path of the filepath
    this->d_directory = path.substr(0, path.find_last_of('/'));

    // Process ASSIMP's root node recursively
    this->ProcessNode(scene->mRootNode, scene);

    // there should always be a 'root node', even if no skeleton exists
    if (!m_skeleton->ImportSkeletonBone(scene->mRootNode)) {
        // fprintf (stderr, "ERROR: Model %s - could not import node tree from mesh\n", path.c_str());
        std::cout << "ERROR: Model could not import node tree from mesh" << std::endl;
    } // endif 

    aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
    // globalTransformation = globalTransformation.Inverse();
    m_skeleton->m_inverse_global = aiMatrix4x4ToGlm(&globalTransformation);
    // if (true) { // debug
    //     std::cout << "m_inverse_global" << glm::to_string(m_skeleton->m_inverse_global) << std::endl;
    // }
    // int numOfBones = m_skeleton->GetNumberOfBones();
    // if (numOfBones > 0) { 
    //     std::cout << "m_skeleton bones: " << numOfBones << std::endl;
    //     // d_bone_location = (GLuint*) malloc( m_skeleton->GetNumberOfBones()* sizeof(GLuint));
    //     for (unsigned int i = 0 ; i < numOfBones; i++) {
    //         char Name[128];
    //         //IKMatrices[i] = glm::mat4(1); 
    //         // memset(Name, 0, sizeof(Name));
    //         // sprintf_s(Name, sizeof(Name), "bones[%d]", i);
    //         //	GLint location = glGetUniformLocation(d_shader->Program, Name);
    //         //					if (location == INVALID_UNIFORM_LOCATION) {
    //         //fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", Name);
    //         //}
    //         //d_bone_location[i] = location;
    //     }
    // }
}

// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::ProcessNode(aiNode* node, const aiScene* scene) { 
    // Process each mesh located at the current node
    for (GLuint i = 0; i < node->mNumMeshes; i++) {
        // The node object only contains indices to index the actual objects in the scene. 
        // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]]; 
        Mesh mesh = this->ProcessMesh(ai_mesh, scene );

        this->d_meshes.push_back(mesh);			
    }
    // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (GLuint i = 0; i < node->mNumChildren; i++) {
        this->ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh* ai_mesh, const aiScene* scene) {
    // Data to fill
    TVecCoord vertices;
    TVecCoord normals;
    std::vector<GLuint> indices;
    std::vector<GLuint> adjacent_indices;
    std::vector<Texture> textures;
    std::vector<VertexWeight> boneWeights;
    std::vector<glm::vec2> textCoordsVert;
    Material material;
    glm::uint numBones = 0; 

    // #pragma region [ Process Vertices ]
    // Walk through each of the mesh's vertices
    for(GLuint i = 0; i < ai_mesh->mNumVertices; i++)
    {
        // Vertex vertex;
        glm::vec3 vector;
        TCoord vectorVert; // We declare a placeholder vector since Assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        TCoord vectorNorm;
        // Positions
        vectorVert[0] = ai_mesh->mVertices[i].x;
        vectorVert[1] = ai_mesh->mVertices[i].y;
        vectorVert[2] = ai_mesh->mVertices[i].z;
        //vertex.Position = vector;
        vertices.push_back(vectorVert);
        // Normals
        if (ai_mesh->HasNormals())
        {
            vectorNorm[0] = ai_mesh->mNormals[i].x;
            vectorNorm[1] = ai_mesh->mNormals[i].y;
            vectorNorm[2] = ai_mesh->mNormals[i].z;
            // vertex.Normal = vector;
            normals.push_back(vectorNorm);
        }
        // Texture Coordinates
        // #ifndef NO_OPENGL
        if(ai_mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = ai_mesh->mTextureCoords[0][i].x; 
            vec.y = ai_mesh->mTextureCoords[0][i].y;
            // vertex.TexCoords = vec;
            textCoordsVert.push_back(vec);
        }
        else
        {
            // vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            textCoordsVert.push_back(glm::vec2(0.0f));
        }

        if (ai_mesh->HasTangentsAndBitangents())
        {
            glm::vec3 tangent;
            tangent.x = ai_mesh->mTangents[i].x;
            tangent.y = ai_mesh->mTangents[i].y;
            tangent.z = ai_mesh->mTangents[i].z;

            // vertex.Tangent = tangent;
        }
        //vertices.push_back(vertex);
    }

    // #pragma region [ Process Faces ]
    // if (d_withAdjacencies)
    //      FindAdjacencies(ai_mesh, adjacent_indices);
    // else
    //Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(GLuint i = 0; i < ai_mesh->mNumFaces; i++)
    {
        aiFace face = ai_mesh->mFaces[i];

        // Retrieve all indices of the face and store them in the indices vector
        for(GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // #pragma region [ Process Materials ]
    // Process materials
    if(ai_mesh->mMaterialIndex >= 0)
    {
        aiMaterial* aiMaterial = scene->mMaterials[ai_mesh->mMaterialIndex];

        aiColor4D ambient;
        glm::vec3 glmAmbient;
        aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient);

        glmAmbient = glm::vec3(0.5f,0.5f,0.5f);//  aiColor4DToGlm(ambient);

        aiColor4D diffuse;
        glm::vec3 glmDiffuse;
        aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
        glmDiffuse = aiColor4DToGlm(diffuse);

        aiColor4D specular;
        glm::vec3 glmSpecular;
        aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specular);
        glmSpecular = aiColor4DToGlm(specular);

        float shininess = 0.0f;
        aiGetMaterialFloat(aiMaterial, AI_MATKEY_SHININESS, &shininess);

        material = Material(glmAmbient,glmDiffuse,glmSpecular,32.0f);

        // We assume a convention for sampler names in the Shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // Diffuse: texture_diffuseN
        // Specular: texture_specularN
        // Normal: texture_normalN

        // 1. Diffuse maps
        std::vector<Texture> diffuseMaps = this->LoadMaterialTextures(aiMaterial, aiTextureType_DIFFUSE, TextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. Specular maps
        std::vector<Texture> specularMaps = this->LoadMaterialTextures(aiMaterial, aiTextureType_SPECULAR, TextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // 3. Normal maps
        std::vector<Texture> normalMaps = this->LoadMaterialTextures(aiMaterial, aiTextureType_NORMALS, TextureType_NORMAL);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // 4. Ref maps
        std::vector<Texture> reflMaps = this->LoadMaterialTextures(aiMaterial, aiTextureType_AMBIENT, TextureType_REFLECTION);
        textures.insert(textures.end(), reflMaps.begin(), reflMaps.end());
    }
    // #pragma endregion  

    // #pragma region [ Process Bones ]
    if(ai_mesh->HasBones())
    {
        int bone_count =  ai_mesh->mNumBones;

        boneWeights.resize(ai_mesh->mNumVertices);

        for (GLuint i = 0 ; i < ai_mesh->mNumBones ; i++) {                
            int BoneIndex = 0;        
            std::string BoneName(ai_mesh->mBones[i]->mName.data);

            if (m_skeleton->boneMapping.find(BoneName) == m_skeleton->boneMapping.end()) {
                // Allocate an index for a new bone
                BoneInfo info;

                BoneIndex = d_numberOfBone++; 
                info.offset = aiMatrix4x4ToGlm(&ai_mesh->mBones[i]->mOffsetMatrix);
                info.index = BoneIndex;
                m_skeleton->boneMapping[BoneName] = info;
            }
            else
                BoneIndex = m_skeleton->boneMapping[BoneName].index;

            for (GLuint j = 0 ; j < ai_mesh->mBones[i]->mNumWeights ; j++) {
                int VertexID =   ai_mesh->mBones[i]->mWeights[j].mVertexId;
                float Weight  = ai_mesh->mBones[i]->mWeights[j].mWeight;   
                for (glm::uint i = 0 ; i < 4 ; i++) {
                    if (boneWeights[VertexID].Weights[i] == 0.0) {
                        boneWeights[VertexID].IDs[i]     = BoneIndex;
                        boneWeights[VertexID].Weights[i] = Weight;
                        break;
                    }        
                }
            }
        }     
    }

    // Return a mesh object created from the extracted mesh data
    return Mesh(vertices,
                indices,
                textures,
                boneWeights,
                adjacent_indices, 
                material,
                textCoordsVert,
                normals);
}

// Checks all material textures of a given type and loads the textures if they're not loaded yet.
// The required info is returned as a Texture struct.
std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName) {
    std::vector<Texture> textures; 
    for(GLuint i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string file_name(str.C_Str());
        // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        GLboolean skip = false;
        for(GLuint j = 0; j < d_textures_loaded.size(); j++)
        {
            if(d_textures_loaded[j].m_file_name == file_name)
            {
                textures.push_back(d_textures_loaded[j]);
                skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }

        if(!skip)
        {   // If texture hasn't been loaded already, load it
            Texture texture(file_name, typeName);
            if (texture.Load(this->d_directory));
            {
                textures.push_back(texture);
                this->d_textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
            //texture.path = str;
        }
    }
    return textures;
}

