#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <vector>
#include <string>

// #include "rapidobj.hpp"
#include "tiny_obj_loader.h"
#include "mesh.h"
#include "shader.h"
#include "util.h"

class Model 
{
public:
    // model data 
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string          dirPath;
    bool                 gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(std::string const &filename, bool gamma = false) 
        : gammaCorrection(gamma) {
        loadModel(filename);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader &shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:    
    // loads a model with supported tinyobjloade extensions from file and stores the resulting meshes in the meshes vector.
    int loadModelTinyObj(std::string const &filename) {
        float bmin[3], bmax[3];
        std::vector<tinyobj::material_t> materials;
        std::map<std::string, GLuint> textures;
        tinyobj::attrib_t inattrib;
        std::vector<tinyobj::shape_t> inshapes;

        // retrieve the directory path of the filepath
        dirPath = filename.substr(0, filename.find_last_of('/'));
        
        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&inattrib, &inshapes, &materials, &warn, &err, filename.c_str(), dirPath.c_str());
        
        if (!warn.empty()) {
            std::cout << "WARN: " << warn << std::endl;
        }
        
        if (!err.empty()) { 
            std::cerr << err << std::endl;
        }

        if (!ret) {
            std::cerr << "Failed to load " << filename << std::endl;
            return EXIT_FAILURE;
        }

        // std::cout << "Parsing time: " << (int)tm.msec() << " [ms]\n";

        std::cout << "# of vertices  = " << (int)(inattrib.vertices.size()) / 3 << "\n";
        std::cout << "# of normals   = " <<  (int)(inattrib.normals.size()) / 3 << "\n";
        std::cout << "# of texcoords = " << (int)(inattrib.texcoords.size()) / 2 << "\n";
        std::cout << "# of materials = " << (int)materials.size() << "\n";
        std::cout << "# of shapes    = " << (int)inshapes.size() << "\n";

        materials.push_back(tinyobj::material_t());

        for (size_t i = 0; i < materials.size(); i++) {
            std::cout << "material[" << (int)i << "].diffuse_texname = " << materials[i].diffuse_texname << "\n";
            std::cout << "material[" << (int)i << "].ambient_texname = " << materials[i].ambient_texname << "\n";
            std::cout << "material[" << (int)i << "].specular_texname = " << materials[i].specular_texname << "\n";
            std::cout << "material[" << (int)i << "].bump_texname = " << materials[i].bump_texname << "\n";
        }
        return EXIT_SUCCESS;
    }

    // rapidobj::Result loadModelRapidObj(std::string const &filename) {
    //     rapidobj::Result result = rapidobj::ParseFile(filename);
    //     if (result.error) {
    //         std::cout << result.error.code.message() << '\n';
    //         return nullptr;
    //     }
    //
    //     bool success = rapidobj::Triangulate(result);
    //     if (!success) {
    //         std::cout << result.error.code.message() << '\n';
    //         return nullptr;
    //     }
    //
    //     size_t num_triangles{};
    //
    //     for (const auto& shape : result.shapes) {
    //         num_triangles += shape.mesh.num_face_vertices.size();
    //     }
    //
    //     std::cout << "Shapes:    " << result.shapes.size() << '\n';
    //     std::cout << "Materials: " << result.materials.size() << '\n';
    //     std::cout << "Material:  " << result.materials[0].name << '\n';
    //     std::cout << "Material ambient:  " << result.materials[0].ambient_texname << '\n';
    //     std::cout << "Material diffuse:  " << result.materials[0].diffuse_texname << '\n';
    //     std::cout << "Material specular:  " << result.materials[0].specular_texname << '\n';
    //     std::cout << "Material bump:  " << result.materials[0].bump_texname << '\n';
    //     std::cout << "Positions: " << result.attributes.positions.size() << '\n';
    //     std::cout << "Texcoords: " << result.attributes.texcoords.size() << '\n';
    //     std::cout << "Normals:   " << result.attributes.normals.size() << '\n';
    //     std::cout << "Triangles: " << num_triangles << '\n';
    //     return result;
    // }

    int loadModel(std::string const &filename) {
        loadModelTinyObj(filename);
        // rapidobj::Result result = loadModelRapidObj(filename);
        // if (result != nullptr) {
        //     // process ASSIMP's root node recursively
        //     // processShape(result);
            
        // }

        return EXIT_SUCCESS;
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    // void processShape(rapidobj::Result &result) {
    //     // process each mesh located at the current node
    //     for (const auto& shape : result.shapes) {
    //         meshes.push_back(processMesh(shape.mesh));
    //     }
    //     // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    // }
    //
    // Mesh processMesh(rapidobj::Mesh &mesh, const aiScene *scene) {
    //     // data to fill
    //     std::vector<Vertex> vertices;
    //     std::vector<unsigned int> indices;
    //     std::vector<Texture> textures;
    //        
    //     // walk through each of the mesh's vertices
    //
    //     // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    //
    //     // process materials
    //
    //     // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    //     // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    //     // Same applies to other texture as the following list summarizes:
    //     // diffuse: texture_diffuseN
    //     // specular: texture_specularN
    //     // normal: texture_normalN
    //
    //     // return a mesh object created from the extracted mesh data
    //     return Mesh(vertices, indices, textures);
    // }

    // vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif