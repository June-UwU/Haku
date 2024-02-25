#pragma once

#include "types.hpp"
#include "defines.hpp"

class Mesh
{
public:
    Mesh();
    ~Mesh();
    void load(const std::string &path);
    void render();

private:
    void processScene(const aiScene* scene);
    void processMesh(const aiMesh* mesh);
    void processVertexs(const aiVector3D& pos);
    void processNormals(const aiVector3D& normal);
    void processUvs(const aiVector3D& uv);
    void processIndices(const aiFace& face);

private:
    std::string meshPath;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<u32> indices;

    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint uvBufferID;
};