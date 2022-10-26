#include "shader.h"
#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>

std::string loadSourceFromFile(const std::string& filename)
{
    std::string source = "";

    std::ifstream in(filename.c_str(),std::ios::in);
    if(!in)
    {
        std::cerr << "File not found " << filename << std::endl;
        return source;
    }

    const int maxBuffersize = 2048;
    char buffer[maxBuffersize];
    while(in.getline(buffer, maxBuffersize))
    {
        source += std::string(buffer) + "\n";
    }

    return source;
}

//--------------------------------------------------------------------------------
bool Shader::loadFromFiles(const std::string& fileV, const std::string& fileF)
{
    std::string vsrc = loadSourceFromFile(fileV);
    std::string fsrc = loadSourceFromFile(fileF);
    return loadSources(vsrc,fsrc);
}
//--------------------------------------------------------------------------------
bool Shader::loadSources(const std::string& vsrc, const std::string& fsrc)
{
    bool allIsOk = true;

    mProgramID = glCreateProgram();

    // vertex shader
    {
        GLuint shaderID = glCreateShader(GL_VERTEX_SHADER);

        const GLchar * arbSource = vsrc.c_str();

        glShaderSource(shaderID, 1, (const GLchar **)&arbSource, 0);
        glCompileShader(shaderID);

        int compiled;
        glGetShaderiv(shaderID,GL_COMPILE_STATUS,&compiled);
        allIsOk = allIsOk && compiled;
        printShaderInfoLog(shaderID);

        if(compiled)
            glAttachShader(mProgramID, shaderID);
    }

    // fragment shader
    {
        GLuint shaderID = glCreateShader(GL_FRAGMENT_SHADER);

        const GLchar * arbSource = fsrc.c_str();

        glShaderSource(shaderID, 1, (const GLchar **)&arbSource, 0);
        glCompileShader(shaderID);
        int compiled;
        glGetShaderiv(shaderID,GL_COMPILE_STATUS,&compiled);

        allIsOk = allIsOk && compiled;
        printShaderInfoLog(shaderID);

        if(compiled)
            glAttachShader(mProgramID, shaderID);
    }

    glLinkProgram(mProgramID);

    int isLinked;
    glGetProgramiv(mProgramID, GL_LINK_STATUS, &isLinked);
    allIsOk = allIsOk && isLinked;
    mIsValid = (isLinked == (int)GL_TRUE);
    if(allIsOk)
        printProgramInfoLog(mProgramID);
    
    return allIsOk;
}
//--------------------------------------------------------------------------------
void Shader::activate(void) const
{
    assert(mIsValid);
    glUseProgram(mProgramID);
}
//--------------------------------------------------------------------------------
void Shader::deactivate(void) const
{
    glUseProgram(0);
}
//--------------------------------------------------------------------------------
int Shader::getUniformLocation(const char* name) const
{
    assert(mIsValid);
    return glGetUniformLocation(mProgramID, name);
}
//--------------------------------------------------------------------------------
void Shader::setSamplerUnit(const char* sampler, int unit) const
{
    activate();
    glUniform1i(getUniformLocation(sampler), unit);
}
//--------------------------------------------------------------------------------
int Shader::getAttribLocation(const char* name) const
{
    assert(mIsValid);
    return glGetAttribLocation(mProgramID, name);
}
//--------------------------------------------------------------------------------
void Shader::dumpInfos() const {
    GLint nbAttrs;
    glGetProgramiv(mProgramID, GL_ACTIVE_ATTRIBUTES, &nbAttrs);

    GLchar name[128];
    GLint size;
    GLenum type;
    for(GLint i=0; i<nbAttrs; ++i) {
        glGetActiveAttrib(mProgramID, i, 128, NULL, &size, &type, name);
        std::cout << "Attrib " << glGetAttribLocation(mProgramID, name) << ": " << name << ", " << size << ", " << int(type) << "\n";
    }
}
//--------------------------------------------------------------------------------
void Shader::printProgramInfoLog(GLuint objectID)
{
    int infologLength = 0, charsWritten = 0;
    GLchar *infoLog;
    glGetProgramiv(objectID,GL_INFO_LOG_LENGTH, &infologLength);
    if(infologLength > 0)
    {
        infoLog = new GLchar[infologLength];
        glGetProgramInfoLog(objectID, infologLength, &charsWritten, infoLog);
        if (charsWritten>0)
            std::cerr << "program info : \n" << infoLog << std::endl;
        delete[] infoLog;
    }
}
//--------------------------------------------------------------------------------
void Shader::printShaderInfoLog(GLuint objectID)
{
    int infologLength = 0, charsWritten = 0;
    GLchar *infoLog;
    glGetShaderiv(objectID,GL_INFO_LOG_LENGTH, &infologLength);
    if(infologLength > 0)
    {
        infoLog = new GLchar[infologLength];
        glGetShaderInfoLog(objectID, infologLength, &charsWritten, infoLog);
        if (charsWritten>0)
            std::cerr << "Shader info : \n" << infoLog << std::endl;
        delete[] infoLog;
    }
}
