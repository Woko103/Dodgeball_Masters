#pragma once

#include <shader.h>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <string>
#include <vector>

using namespace std;
using namespace glm;

struct Vertex
{
        vec3 Position; // Position of the vertex
        vec3 Normal; // Normal vector of the vertex
        vec2 TextureCoords; // Texture coordinates for that vertex
};

struct  Texture
{
        unsigned int id; // Id of the texture
        string type; // Type of the texture (diffuse, specular, metallic etc...)
        string path;
};

class RMesh
{
        // Render data
        unsigned int VAO, VBO, EBO;
        void setupMesh();

public:
        // Mesh data
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        RMesh() = default;
        void attachData(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        void Draw(Shader* shader);        
        void saveData(ofstream& writeFile);
        void loadData(ifstream& readFile);
        void loadTextures(string directory);
};
