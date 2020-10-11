#include <MyModel.h>
#include <filesystem>

MyModel::MyModel(GLFWwindow* mainWindow, Shader* shader, const char* path)
{
        this->shader = shader;
        window = mainWindow;

        // Initialise transparecy of the model
        transparency = 1.0f;

        // retrieve the directory path of the filepath
        directory = path;
        directory = directory.substr(0, directory.find_last_of('/'));

        // Check if the animation files filled with data are existant
        // otherwise we have to load the model the normal way
        if (filesystem::exists(directory + "/Idle.gg"))
        {
            hasAnimation = true;
            frame = 0;
            animationTime = 0;
            animationState = Idle; // Default animation state if the object has animation
            ifstream readFile;

            // Reading animation data
            readAnimationData(readFile, Idle);
            readAnimationData(readFile, Running);
            readAnimationData(readFile, Throwing);
            readAnimationData(readFile, Catching);
            readAnimationData(readFile, Dashing);

        }
        else
            loadModel(path);
}

void MyModel::draw(mat4 model)
{
    updateAnimationState();
    shader->setMat4("model", model); // Passing transformation matrix to the shader
    shader->setFloat("transparency", transparency);

    if (hasAnimation)
    {
        switch (animationState)
        {
            case Idle:
                if (animationTime > 0.03)
                {
                    animationTime = 0;
                    frame += numMaterials;
                    if (frame >= idleAnim.size())
                        frame = 0;
                }
                for (uint32_t i = 0; i < numMaterials; i++)
                    idleAnim.at(frame + i).Draw(shader);
                animationTime += 0.0167;
            break;

            case Running:
                if (animationTime > 0.02)
                {
                    animationTime = 0;
                    frame += numMaterials;
                    if (frame >= runAnim.size())
                        frame = 0;
                }
                for (uint32_t i = 0; i < numMaterials; i++)
                    runAnim.at(frame + i).Draw(shader);
                animationTime += 0.0167;
            break;

            case Throwing:
                if (animationTime > 0.03)
                {
                    animationTime = 0;
                    frame += numMaterials;
                    if (frame >= throwAnim.size())
                        frame = 0;
                }
                for (uint32_t i = 0; i < numMaterials; i++)
                    throwAnim.at(frame + i).Draw(shader);
                animationTime += 0.0167;
            break;

            case Catching:
                if (animationTime > 0.03)
                {
                    animationTime = 0;
                    frame += numMaterials;
                    if (frame >= catchAnim.size())
                        frame = 0;
                }
                for (uint32_t i = 0; i < numMaterials; i++)
                    catchAnim.at(frame + i).Draw(shader);
                animationTime += 0.0167;
            break;

            case Dashing:
                if (animationTime > 0.03)
                {
                    animationTime = 0;
                    frame += numMaterials;
                    if (frame >= dashAnim.size())
                        frame = 0;
                }
                for (uint32_t i = 0; i < numMaterials; i++)
                    dashAnim.at(frame + i).Draw(shader);
                animationTime += 0.0167;
            break;

            default:
            break;
        }
    }
    else
    {
        for (int i = 0; i < meshes.size(); i++)
            meshes.at(i).Draw(shader);
    }

}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void MyModel::loadModel(string const &path)
{
    // Checking pathing to load any mesh that will have animation
    string filePath(path);
    // e.g."meshes/BeepBot/BeepBot.obj"

    Assimp::Importer importer;

    // Check if any of the animation folders exists. If it does, the model has animation
    if (filesystem::exists(directory + "/idle/"))
    {
        string idleAnimPath = directory + "/idle/";
        string runAnimPath = directory + "/run/";
        string throwAnimPath = directory + "/throw/";
        string catchAnimPath = directory + "/catch/";
        string dashAnimPath = directory + "/dash/";

        hasAnimation = true;
        animationState = Idle; // Default animation state if the object has animation

        loadAnimation(idleAnimPath, Idle);
        loadAnimation(runAnimPath, Running);
        loadAnimation(throwAnimPath, Throwing);
        loadAnimation(catchAnimPath, Catching);
        loadAnimation(dashAnimPath, Dashing);
    }
    else  // Importing mesh WITHOUT animations
    {
        hasAnimation = false;

        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene, None);
    }
}

void MyModel::processNode(aiNode* node, const aiScene* scene, int animType)
{
        // We loop all the meshes for a single node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            RMesh processedMesh = processMesh(mesh, scene);
            // Depending of the animation type, we will save the mesh to a different section
            switch (animType)
            {
                case Idle:
                    idleAnim.push_back(processedMesh);
                break;

                case Running:
                    runAnim.push_back(processedMesh);
                break;

                case Throwing:
                    throwAnim.push_back(processedMesh);
                break;

                case Catching:
                    catchAnim.push_back(processedMesh);
                break;

                case Dashing:
                    dashAnim.push_back(processedMesh);
                break;

                case None:
                    meshes.push_back(processedMesh);
                break;

                default:
                    break;
            }
        }

        // We loop throughout all the childs of a single node, and call again processNode to get the meshes of that node
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, animType);
        }

}

RMesh MyModel::processMesh(aiMesh* mesh,const aiScene* scene)
{
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        RMesh fMesh;

        // Process vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            // Process vertex positions, normals and texture coordinates
            vertex.Position = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.Normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

            if (mesh->mTextureCoords[0]) // We check if the mesh contains any textures coordinates at all
            {
                    vertex.TextureCoords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else // If it doesn't ...
                    vertex.TextureCoords = vec2(0.0f, 0.0f);

            vertices.push_back(vertex); // Storing the processed vertex from the mesh into our "vertices" vector
        }

        // Process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) // We loop for each of the faces that the mesh is containing
        {
                aiFace face = mesh->mFaces[i];

                for (unsigned int j = 0; j < face.mNumIndices; j++) // We loop throughout each of the indices a single face contains
                        indices.push_back(face.mIndices[j]); // Store that index inside our "indices" vector
        }

        // Process textures/material
        if (!texturesProcessed)
        {
            if (mesh->mMaterialIndex >= 0) // Check if the mesh has a material attached
            {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; //  We retrieve the material stored in the mesh

                vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); // We extract the diffuseMaps from the material extracted from our single mesh
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            }
        }

        fMesh.attachData(vertices, indices, textures);

    return fMesh;
}

vector<Texture> MyModel::loadMaterialTextures(aiMaterial* material, aiTextureType type, const char* typeName)
{
    vector<Texture> textures;

    for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        Texture texture;
        texture.id  = TextureFromFile(str.C_Str(), directory);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }

    return textures;
}

unsigned int TextureFromFile(const char* path, const string &directory, bool gamma)
{
        string filename(path);
        if (filename.find_last_of("/") == -1)
            filename = filename.substr(filename.find_last_of("\\") + 1);
        else
            filename = filename.substr(filename.find_last_of("/") + 1);

        filename = directory + "/textures/" + filename;

        //cout << "Texture path: " << filename << endl;

        unsigned int textureID;
        glGenTextures(1, &textureID); // Generating texture

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

        if (data)
        {
                GLenum format;

                if (nrComponents == 1)
                        format  = GL_RED;
                else if (nrComponents == 3)
                        format = GL_RGB;
                else if (nrComponents == 4)
                        format = GL_RGBA;

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glBindTexture(GL_TEXTURE_2D, textureID); // Binding texture
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // Load texture
                glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(data);
        }
        else
        {
                //cout << "Texture failed to load at path: " << path << endl;
                stbi_image_free(data);
        }

        return textureID;
}

void MyModel::loadAnimation(string animPath, int type)
{
    string animType;

    switch (type)
    {
        case Idle:
            animType = "Idle";
        break;

        case Running:
            animType = "Run";
        break;

        case Throwing:
            animType = "Throw";
        break;

        case Catching:
            animType = "Catch";
        break;

        case Dashing:
            animType = "Dash";
        break;

        default:
        break;
    }

    bool lastFileFound = false;
    Assimp::Importer importer;

    for (int i = 0; i < 1000; i++) // Setting a 1000 as a limit of frames per animation
    {
        string pathToImport = animPath;
        if (i >= 0 && i < 10) //00000#
        {
            pathToImport += animType + "_00000" + to_string(i) + ".obj";
        }
        else if (i >= 10 && i <= 99) //0000##
        {
            pathToImport += animType + "_0000" + to_string(i) + ".obj";
        }
        else // 000###
        {
            pathToImport += animType + "_000" + to_string(i) + ".obj";
        }

        //cout << "Importing path #" << i << ": " << pathToImport << endl;

        const aiScene* scene = importer.ReadFile(pathToImport, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            //cout << "File #" << i << " not found." << endl;
            lastFileFound = true;
        }
        else // If file is found, proceed saving its data ...
        {
            numMaterials = scene->mRootNode->mNumChildren;
            //cout << "Total material: " << numMaterials << endl;
            // process ASSIMP's root node recursively
            processNode(scene->mRootNode, scene, type);

            texturesProcessed = true;
        }

        if (lastFileFound) // In case last file is processed, exit the loop
        {

            ofstream writeFile;

            size_t idleAnimSize = idleAnim.size();
            size_t runAnimSize = runAnim.size();
            size_t throwAnimSize = throwAnim.size();
            size_t catchAnimSize = catchAnim.size();
            size_t dashAnimSize = dashAnim.size();

            switch (type)
            {
                case Idle:
                    // Write animation data to Idle.gg
                    writeFile.open(directory + "/Idle.gg", ios::out | ios::binary);
                    // Write number of materials
                    writeFile.write((const char*)&numMaterials, sizeof(int));
                    // Write animation sizes
                    writeFile.write((const char*)&idleAnimSize, sizeof(size_t));
                    for (uint32_t i = 0; i < idleAnim.size(); i++)
                        idleAnim[i].saveData(writeFile);
                    writeFile.close();
                break;

                case Running:
                    // Write animation data to Run.gg
                    writeFile.open(directory + "/Run.gg", ios::out | ios::binary);
                    // Write number of materials
                    writeFile.write((const char*)&numMaterials, sizeof(int));
                    // Write animation sizes
                    writeFile.write((const char*)&runAnimSize, sizeof(size_t));
                    for (uint32_t i = 0; i < runAnim.size(); i++)
                        runAnim[i].saveData(writeFile);
                    writeFile.close();
                break;

                case Throwing:
                    // Write animation data to Throw.gg
                    writeFile.open(directory + "/Throw.gg", ios::out | ios::binary);
                    // Write number of materials
                    writeFile.write((const char*)&numMaterials, sizeof(int));
                    // Write animation sizes
                    writeFile.write((const char*)&throwAnimSize, sizeof(size_t));
                    for (uint32_t i = 0; i < throwAnim.size(); i++)
                        throwAnim[i].saveData(writeFile);
                    writeFile.close();
                break;

                case Catching:
                    // Write animation data to Catch.gg
                    writeFile.open(directory + "/Catch.gg", ios::out | ios::binary);
                    // Write number of materials
                    writeFile.write((const char*)&numMaterials, sizeof(int));
                    // Write animation sizes
                    writeFile.write((const char*)&catchAnimSize, sizeof(size_t));
                    for (uint32_t i = 0; i < catchAnim.size(); i++)
                        catchAnim[i].saveData(writeFile);
                    writeFile.close();
                break;

                case Dashing:
                    // Write animation data to Dash.gg
                    writeFile.open(directory + "/Dash.gg", ios::out | ios::binary);
                    // Write number of materials
                    writeFile.write((const char*)&numMaterials, sizeof(int));
                    // Write animation sizes
                    writeFile.write((const char*)&dashAnimSize, sizeof(size_t));
                    for (uint32_t i = 0; i < dashAnim.size(); i++)
                        dashAnim[i].saveData(writeFile);
                    writeFile.close();
                break;

                default:
                break;
            }

            break; // Leaving for loop
        }
    }
}

void MyModel::readAnimationData(ifstream& readFile, int animType)
{
    //cout << "\tReading animation data..." << endl;
    size_t size;

    int aux;

    switch (animType)
    {
        case Idle:
            readFile.open(directory + "/Idle.gg");
            readFile.read((char*)&numMaterials, sizeof(int));
            readFile.read((char*)&size, sizeof(size_t));
            aux = 0;

            //cout << "Number of materials: " << numMaterials << endl;

            for (uint32_t i = 0; i < size; i++)
            {
                RMesh myMesh = RMesh();
                myMesh.loadData(readFile); // Loading mesh data
                if (i < numMaterials)
                    myMesh.loadTextures(directory); // Loading texture once

                idleAnim.push_back(myMesh); // Saving mesh inside our animation vector
            }

            // Setting textures values
            for (uint32_t i = numMaterials; i < idleAnim.size(); i++)
            {
              idleAnim[i].textures = idleAnim[aux].textures;
              aux++;

              if (aux == numMaterials)
                  aux = 0;
            }

            readFile.clear();
            readFile.close();

        break;

        case Running:
            readFile.open(directory + "/Run.gg");
            readFile.read((char*)&numMaterials, sizeof(int));
            readFile.read((char*)&size, sizeof(size_t));
            aux = 0;

            for (uint32_t i = 0; i < size; i++)
            {
                RMesh myMesh = RMesh();
                myMesh.loadData(readFile); // Loading mesh data
                runAnim.push_back(myMesh); // Saving mesh inside our animation vector
            }

            // Setting textures values
            for (uint32_t i = 0; i < runAnim.size(); i++)
            {
              runAnim[i].textures = idleAnim[aux].textures;
              aux++;

              if (aux == numMaterials)
                  aux = 0;
            }

            readFile.clear();
            readFile.close();
        break;

        case Throwing:
            readFile.open(directory + "/Throw.gg");
            readFile.read((char*)&numMaterials, sizeof(int));
            readFile.read((char*)&size, sizeof(size_t));
            aux = 0;

            for (uint32_t i = 0; i < size; i++)
            {
                RMesh myMesh = RMesh();
                myMesh.loadData(readFile); // Loading mesh data
                throwAnim.push_back(myMesh); // Saving mesh inside our animation vector
            }

            // Setting textures values
            for (uint32_t i = 0; i < throwAnim.size(); i++)
            {
              throwAnim[i].textures = idleAnim[aux].textures;
              aux++;

              if (aux == numMaterials)
                  aux = 0;
            }

            readFile.clear();
            readFile.close();
        break;

        case Catching:
            readFile.open(directory + "/Catch.gg");
            readFile.read((char*)&numMaterials, sizeof(int));
            readFile.read((char*)&size, sizeof(size_t));

            for (uint32_t i = 0; i < size; i++)
            {
                RMesh myMesh = RMesh();
                myMesh.loadData(readFile); // Loading mesh data
                catchAnim.push_back(myMesh); // Saving mesh inside our animation vector
            }

            // Setting textures values
            for (uint32_t i = 0; i < catchAnim.size(); i++)
            {
              catchAnim[i].textures = idleAnim[aux].textures;
              aux++;

              if (aux == numMaterials)
                  aux = 0;
            }

            readFile.clear();
            readFile.close();
        break;

        case Dashing:
            readFile.open(directory + "/Dash.gg");
            readFile.read((char*)&numMaterials, sizeof(int));
            readFile.read((char*)&size, sizeof(size_t));

            for (uint32_t i = 0; i < size; i++)
            {
                RMesh myMesh = RMesh();
                myMesh.loadData(readFile); // Loading mesh data
                dashAnim.push_back(myMesh); // Saving mesh inside our animation vector
            }

            // Setting textures values
            for (uint32_t i = 0; i < dashAnim.size(); i++)
            {
              dashAnim[i].textures = idleAnim[aux].textures;
              aux++;

              if (aux == numMaterials)
                  aux = 0;
            }

            readFile.clear();
            readFile.close();
        break;

        default:
        break;
    }
}

void MyModel::updateAnimationState()
{
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // Idle
    {
        frame = 0;
        animationTime = 0;
        animationState = Idle;
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // Running
    {
        frame = 0;
        animationTime = 0;
        animationState = Running;
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // Throwing
    {
        frame = 0;
        animationTime = 0;
        animationState = Throwing;
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) // Catching
    {
        frame = 0;
        animationTime = 0;
        animationState = Catching;
    }

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) // Dashing
    {
        frame = 0;
        animationTime = 0;
        animationState = Dashing;
    }
}

void MyModel::setState(int state)
{

    switch (state)
    {
        case Idle:
            animationState = Idle;
        break;

        case Running:
            animationState = Running;
        break;

        case Throwing:
            animationState = Throwing;
        break;

        case Catching:
            animationState = Catching;
        break;

        case Dashing:
            animationState = Dashing;
        break;

        default:
        break;
    }

    frame = 0;
    animationTime = 0;
}

int MyModel::getFrame(){
    return frame;
}

int MyModel::getAnimationSize(int animationType)
{
    switch (animationType)
    {
        case Idle:
            return idleAnim.size();
        break;

        case Running:
            return runAnim.size();
        break;

        case Throwing:
            return throwAnim.size();
        break;

        case Catching:
            return catchAnim.size();
        break;

        case Dashing:
            return dashAnim.size();
        break;

        default:
        break;
    }
}

void MyModel::setTransparency(float transparencyToSet)
{
    transparency = transparencyToSet;
}
