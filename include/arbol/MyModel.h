#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <shader.h>
#include <stb_image.h>
#include <RMesh.h>
#include <MyEntity.h>
#include <vector>
#include <string>

using namespace std;

unsigned int TextureFromFile(const char* path, const string &directory, bool gamma = false);

enum AnimationState {Idle, Running, Throwing, Catching, Dashing, None};

class MyModel : public MyEntity
{
        float transparency;

        Shader* shader; // Shader reference
        GLFWwindow* window; // Window reference
        vector<RMesh> meshes;

        // Vectors containing in each position the meshes that form up a model
        vector<RMesh> idleAnim;
        vector<RMesh> runAnim;
        vector<RMesh> throwAnim;
        vector<RMesh> catchAnim;
        vector<RMesh> dashAnim;

        int frame; // Current frame to be displayed from a certain animation of the model
        int numMaterials; // Total number of material used in a model
        float animationTime; // Used to set the time between each frame of the animation
        bool hasAnimation; // Boolean that represents if a certain model has animation or not
        string directory; // Model's directory
        bool texturesProcessed = false;

        // Functions
        void loadAnimation(string animPath, int animType);
        void readAnimationData(ifstream& readFile, int animType);
        void loadModel(string const& path);
        void processNode(aiNode* node, const aiScene* scene, int animType);
        RMesh processMesh(aiMesh* mesh, const aiScene* scene);
        vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const char* typeName);

public:
        AnimationState animationState; // Current animation state of the model

        MyModel(GLFWwindow* mainWindow, Shader* shader, const char* path);
        void updateAnimationState();
        void draw(mat4 model) override;
        void setState(int);
        int getFrame();
        int getAnimationSize(int);

        void setTransparency(float transparencyToSet);
};
