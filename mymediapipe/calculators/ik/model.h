#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

class Model
{
public:
    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model() {}
    Model(GLchar* path);
    
    // Draws the model, and thus all its meshes
    void Draw(Shader shader);
  
private:
    /*  Model Data  */
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    
    /*  Functions   */
    // Loads a model and stores the meshes in the meshes vector.
    void LoadModel(std::string path);
    
    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void ProcessNode(aiNode* node, const aiScene* scene);
    
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
};
#endif