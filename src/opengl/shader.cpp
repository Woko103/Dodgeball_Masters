#include <shader.h>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
        // 1. Retrieve the vertex/fragment source code from filePath
        string vertexCode;
        string fragmentCode;

        ifstream vShaderFile;
        ifstream fShaderFile;

        // Ensure ifstream objects can throw exceptions
        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

        try
        {
                // Open files
                vShaderFile.open(vertexPath);
                fShaderFile.open(fragmentPath);
                stringstream vShaderStream, fShaderStream;

                // Read file's buffer contents into streams
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();

                // Close file handlers
                vShaderFile.close();
                fShaderFile.close();

                // Convert stream into string
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
        }
        catch(ifstream::failure e)
        {
                cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
        }
        
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        
        // 2. Compile shaders
        unsigned int vertex, fragment;

        // Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // Print if there are any errors regarding the creation of the vertex shader
        checkCompileErrors(vertex, "VERTEX");
        
        // Fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        
        // Print if there are any errors regarding the creation of the fragment shader
        checkCompileErrors(fragment, "FRAGMENT");

        // Shader program
        id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        
        // Print if there are any linking problems
        checkCompileErrors(id, "PROGRAM");

        // Delete shaders as they're liked already in our program 
        glDeleteShader(vertex);
        glDeleteShader(fragment);
}

void Shader::use()
{
        glUseProgram(id);
}

void Shader::setBool(const string &name, bool value) const
{
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setInt(const string &name, int value) const
{
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const string &name, float value) const
{
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setVec2(const string &name, vec2 value) const
{
        glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
}

void Shader::setVec3(const string &name, vec3 value) const 
{
        glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec4(const string &name, vec4 value) const
{
        glUniform4f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const string &name, mat4 value) const
{
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::checkCompileErrors(unsigned int shader, string type)
{
        int success;
        char infoLog[512];

        if (type == "PROGRAM")
        {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                
                if (!success)
                {
                        glGetProgramInfoLog(shader, 512, NULL, infoLog);
                        cout << "ERROR::SHADER:: " << shader << "::LINKING FAILED: " << endl;
                        cout << infoLog << endl;
                }
        }
        else
        {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

                if (!success)
                {
                        glGetShaderInfoLog(shader, 512, NULL, infoLog);
                        cout << "ERROR::SHADER:: " << shader << "::COMPILATION PROBLEM: " << endl;
                        cout << infoLog << endl;
                }
        }
}