#pragma once

#include "glm/glm.hpp"
#include <GL/glew.h>

#include "Engine/Resource.h"

class ShaderOpenGL : public Resource
{
public:
	ShaderOpenGL() = default;
	ShaderOpenGL(std::string name, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	explicit ShaderOpenGL(std::string name);

	ShaderOpenGL(std::string dir, void* args);

	void* Create(std::string name, const char* vertexPath, const char* fragmentPath, const char* geometryPath);

	~ShaderOpenGL() override;

	unsigned int CompileShader(unsigned int type, const std::string& source);

	void CheckCompileErrors(GLuint shader, std::string type);

	unsigned int GetID() const;

	// Static functions
	static void Use(unsigned int shader_ID);

	static void setBool(unsigned int shader_ID, const std::string& name, bool value);

	static void setInt(unsigned int shader_ID, const std::string& name, int value);

	static void setFloat(unsigned int shader_ID, const std::string& name, float value);

	static void setVec2(unsigned int shader_ID, const std::string& name, const glm::vec2& value);

	static void setVec2(unsigned int shader_ID, const std::string& name, float x, float y);

	static void setVec3(unsigned int shader_ID, const std::string& name, const glm::vec3& value);

	static void setVec3(unsigned int shader_ID, const std::string& name, float x, float y, float z);

	static void setVec4(unsigned int shader_ID, const std::string& name, const glm::vec4& value);

	static void setVec4(unsigned int shader_ID, const std::string& name, float x, float y, float z, float w);

	static void setMat2(unsigned int shader_ID, const std::string& name, const glm::mat2& mat);

	static void setMat3(unsigned int shader_ID, const std::string& name, const glm::mat3& mat);

	static void setMat4(unsigned int shader_ID, const std::string& name, const glm::mat4& mat);

	// overrided cast operator to get the shader ID
	operator unsigned int() const
	{
		return shader_ID;
	}

private:
	unsigned int shader_ID = 0;
};
