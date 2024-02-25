#include "mesh.hpp"
#include "core/logger.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Mesh::Mesh();
Mesh::~Mesh();
void Mesh::load(const std::string &path) {
    meshPath = path;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
    aiProcess_CalcTangentSpace       |
    aiProcess_Triangulate            |
    aiProcess_JoinIdenticalVertices  |
    aiProcess_SortByPType);

    if(nullptr == scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_CRITICAL("ASSIMP:: %s", importer.GetErrorString());
        return;
    }
        
}
void Mesh::render() {

}

void Mesh::processScene(const aiScene* scene) {
    for(u32 i = 0; i < scene->mNumMeshes; ++i) {
        processMesh(scene->mMeshes[i]);
    }
}
}

void Mesh::processMesh(const aiMesh* mesh) {
    for(u32 i = 0; i < mesh->mNumVertices; ++i) {
        processVertexs(mesh->mVertices[i]);

        if(true == mesh->hasNormals()) {
            processNormals(mesh->mNormals[i]);
        }
        else {
            // TODO(June) : generate normals from vertex if no normals is not provided
            normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        }

        if(true == mesh->HasTextureCoords(0)) {
            processUvs(mesh->mTextureCoords[0][i]);
        }
        else {
            uvs.push_back(glm::vec2(0.0f, 0.0f));
        }
    }

    for(u32 i = 0; i < mesh->mNumFaces; ++i) {
        processIndices(mesh->mFaces[i]);
    }
}

void Mesh::processVertexs(const aiVector3D& pos) {
    vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
}

void Mesh::processNormals(const aiVector3D& normal) {
    normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
}

void Mesh::processUvs(const aiVector3D& uv) {
    uvs.push_back(glm::vec2(uv.x, uv.y));
}

void Mesh::processIndices(const aiFace& face) {
    for(u32 i = 0; i < face.mNumIndices; ++i) {
        indices.push_back(face.mIndices[i]);
    }
}

