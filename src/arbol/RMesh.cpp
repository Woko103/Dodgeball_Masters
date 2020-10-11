#include <RMesh.h>

void RMesh::attachData(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{   
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
}

void RMesh::setupMesh()
{
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        // Texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoords));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
}

void RMesh::Draw(Shader* shader)
{
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // Activating texture unit 
        glBindTexture(GL_TEXTURE_2D, textures.at(i).id); // Bind the texture 
        shader->use();
        shader->setInt(("materal.diffuse"), i); // Sending the texture unit, where we've stored the texture (GL_TEXTURE0 in that case)
        shader->setVec3("material.ambient", vec3(1.0f, 0.5f, 0.31f));
        shader->setVec3("material.specular", vec3(0.1f));
        shader->setFloat("material.shininess", 4.0f);
    }

    // Draw the mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Resetting the active texture
    glActiveTexture(GL_TEXTURE0);
}

void RMesh::saveData(ofstream& writeFile)
{
    size_t numV = vertices.size();
    size_t numI = indices.size();
    size_t numT = textures.size();

    // Writing vector's sizes of the mesh to the binary file
    writeFile.write((const char*)&numV, sizeof(size_t));
    writeFile.write((const char*)&numI, sizeof(size_t));
    writeFile.write((const char*)&numT, sizeof(size_t));

    // Writing vertices of the mesh to the binary file
    for (uint32_t i = 0; i < vertices.size(); i++)
    {
        writeFile.write((const char*)&vertices[i].Position, sizeof(vec3));
        writeFile.write((const char*)&vertices[i].Normal, sizeof(vec3));
        writeFile.write((const char*)&vertices[i].TextureCoords, sizeof(vec2));
    }

    // Writing indices of the mesh to the binary file
    writeFile.write((const char*)&indices[0], indices.size() * sizeof(unsigned int));

    // Writing textures of the mesh to the binary file
    for (uint32_t i = 0; i < textures.size(); i++)
    {
        size_t pathSize = (textures[i].path).size();
        size_t typeSize = (textures[i].type).size();

        writeFile.write((const char*)&textures[i].id, sizeof(textures[i].id));
        writeFile.write((const char*)&pathSize, sizeof(size_t)); // Path size string
        writeFile.write((const char*)&typeSize, sizeof(size_t)); // Type size string
        writeFile.write(textures[i].path.c_str(), pathSize); // Writing path string
        writeFile.write(textures[i].type.c_str(), typeSize); // Writing type string
    }
}

void RMesh::loadData(ifstream& readFile)
{   
    // Reading vector's sizes 
    size_t numV, numI, numT;
    readFile.read((char*)&numV, sizeof(size_t));
    readFile.read((char*)&numI, sizeof(size_t));
    readFile.read((char*)&numT, sizeof(size_t));

    vertices.reserve(numV * sizeof(Vertex));
    indices.reserve(numI * sizeof(unsigned int));
    textures.reserve(numT * sizeof(Texture));
    
    // Reading vertices
    for (uint32_t i = 0; i < numV; i++)
    {
        Vertex vertex;

        readFile.read((char*)&vertex.Position, sizeof(vec3));
        readFile.read((char*)&vertex.Normal, sizeof(vec3));
        readFile.read((char*)&vertex.TextureCoords, sizeof(vec2));

        vertices.push_back(vertex);
    }

    // Reading indices
    for (uint32_t i = 0; i < numI; i++)
    {
        unsigned int index;

        readFile.read((char*)&index, sizeof(unsigned int));

        indices.push_back(index);
    }

    // Reading textures
    for (uint32_t i = 0; i < numT; i++)
    {
        Texture tex;
        size_t pathSize, typeSize;

        readFile.read((char*)&tex.id, sizeof(unsigned int));
        readFile.read((char*)&pathSize, sizeof(size_t));
        readFile.read((char*)&typeSize, sizeof(size_t));
        tex.path.resize(pathSize);
        tex.type.resize(typeSize);
        readFile.read(&tex.path[0], tex.path.size());
        readFile.read(&tex.type[0], tex.type.size());
        textures.push_back(tex);
    }

    setupMesh();
}

void RMesh::loadTextures(string directory)
{  
    for (uint32 i = 0; i < textures.size(); i++)
    {
        string filename(textures[i].path);

        if (filename.find_last_of("/") == -1)
            filename = filename.substr(filename.find_last_of("\\") + 1);
        else 
            filename = filename.substr(filename.find_last_of("/") + 1);
            
        filename = directory + "/textures/" + filename;

        //cout << "ID: " << textures[i].id << endl;
        //cout << "Path: " << filename << endl;

        glGenTextures(1, &textures[i].id); // Generating texture

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

                glBindTexture(GL_TEXTURE_2D, textures[i].id); // Binding texture
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // Load texture
                glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(data);
        }
        else 
        {
                //cout << "Texture failed to load at path: " << filename << endl;
                stbi_image_free(data);
        }
    }
}