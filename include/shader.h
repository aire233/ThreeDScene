// shader.h
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // 包含 GLAD
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;

    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath) {
        // 1. 从文件路径中获取顶点/片段着色器源码
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // 确保 ifstream 对象可以抛出异常
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // 打开文件
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // 读取文件的缓冲内容到数据流中
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // 关闭文件处理器
            vShaderFile.close();
            fShaderFile.close();
            // 转换数据到 string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. 编译着色器
        unsigned int vertex, fragment;
        int success;
        char infoLog[512];
        // 顶点着色器
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // 检查编译错误
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // 片段着色器
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // 检查编译错误
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // 着色器程序
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        // 检查链接错误
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        // 删除着色器
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    // 使用/激活着色器
    void use() {
        glUseProgram(ID);
    }
    // uniform 工具函数
    void setBool(const std::string& name, bool value) const {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    void setInt(const std::string& name, int value) const { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    void setFloat(const std::string& name, float value) const { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    void setVec3(const std::string& name, const glm::vec3& value) const { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const { 
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); 
    }
};

#endif
