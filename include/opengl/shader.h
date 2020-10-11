#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
using namespace glm;

class Shader
{
public:
        // Program ID
        unsigned int id;

        // Constructor that reads and builds the shader
        Shader(const char* vertexPath, const char* fragmentPath);
        // use/activate the shader
        void use();
        // utility uniform functions
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec2(const std::string &name, vec2 value) const;
        void setVec3(const std::string &name, vec3 value) const;
        void setVec4(const std::string &name, vec4 value) const;
        void setMat4(const std::string &name, mat4 value) const;
        void checkCompileErrors(unsigned int shader, std::string type);
};