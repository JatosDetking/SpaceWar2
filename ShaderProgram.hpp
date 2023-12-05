#pragma once
#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <glad\glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/matrix.hpp>

class ShaderProgram
{
public:
	unsigned int ID;

	ShaderProgram(const char* vertexPath, const char* fragmentPath)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();

		std::string vShaderCode = vShaderStream.str();
		std::string fShaderCode = fShaderStream.str();

		// 2. compile shaders
		int success;
		char infoLog[512];

		// vertex Shader
		unsigned vertex = glCreateShader(GL_VERTEX_SHADER);
		try
		{
			const GLchar* vertSrc = vShaderCode.c_str();
			glShaderSource(vertex, 1, &vertSrc, NULL);
			glCompileShader(vertex);
			// print compile errors if any
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				std::stringstream err("Vertex shader compilation error: ");
				err << infoLog;
				throw std::exception(err.str().c_str());
			};

			// fragment Shader
			unsigned fragment = glCreateShader(GL_FRAGMENT_SHADER);
			try
			{
				const GLchar* fragSrc = fShaderCode.c_str();
				glShaderSource(fragment, 1, &fragSrc, NULL);
				glCompileShader(fragment);
				// print compile errors if any
				glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(fragment, 512, NULL, infoLog);
					std::stringstream err("Fragment shader compilation error: ");
					err << infoLog;
					throw std::exception(err.str().c_str());
				};

				// shader Program
				ID = glCreateProgram();
				try
				{
					glAttachShader(ID, vertex);
					glAttachShader(ID, fragment);
					glLinkProgram(ID);
					// print linking errors if any
					glGetProgramiv(ID, GL_LINK_STATUS, &success);
					if (!success)
					{
						glGetProgramInfoLog(ID, 512, NULL, infoLog);
						std::stringstream err("Shader program link error: ");
						err << infoLog;
						throw std::exception(err.str().c_str());
					}

					glDeleteShader(vertex);
					glDeleteShader(fragment);
				}
				catch (...)
				{
					glDeleteProgram(ID);
					throw;
				}
			}
			catch (...)
			{
				glDeleteShader(fragment);
				throw;
			}
		}
		catch (...)
		{
			glDeleteShader(vertex);
			throw;
		}
	}
	// use/activate the shader
	void use()
	{
		glUseProgram(ID);
	}

	// utility uniform functions
	void setBool(const char* name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const char* name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const char* name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const char* name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name), 1, &value[0]);
	}
	void setVec2(const char* name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const char* name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
	}
	void setVec3(const char* name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const char* name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name), 1, &value[0]);
	}
	void setVec4(const char* name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const char* name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const char* name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const char* name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
	}
};


#endif