#include "pch.h"

#include "ShaderOpenGL.h"

#include "GLFW/glfw3.h"
#include "gl/glew.h"

ShaderOpenGL::ShaderOpenGL(std::string name, const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	Create(name, vertexPath, fragmentPath, geometryPath);
}

ShaderOpenGL::ShaderOpenGL(std::string name)
{
	this->name = name;
}

ShaderOpenGL::ShaderOpenGL(std::string dir, void* args)
{
	Create(dir, dir.c_str(), (const char*)args, nullptr);
}

void* ShaderOpenGL::Create(std::string name, const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	this->name = name;
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
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
		// convert stream into string
		vertexCode   = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// if geometry shader path is present, also load a geometry shader
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		S_ASSERT(false, "File failed to read")
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertex;
	unsigned int fragment;

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char* gShaderCode = geometryCode.c_str();
		geometry                = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		glCompileShader(geometry);
		CheckCompileErrors(geometry, "GEOMETRY");
	}
	// shader Program
	shader_ID = glCreateProgram();
	glAttachShader(shader_ID, vertex);
	glAttachShader(shader_ID, fragment);
	if (geometryPath != nullptr)
		glAttachShader(shader_ID, geometry);
	glLinkProgram(shader_ID);
	CheckCompileErrors(shader_ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);

	return this;
}

ShaderOpenGL::~ShaderOpenGL()
{
	glDeleteProgram(shader_ID);
}

unsigned int ShaderOpenGL::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id   = glCreateShader(type);
	const GLchar* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		auto* errorMessage = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, errorMessage);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "fragment") << "Shader" << std::endl;
		std::cout << errorMessage << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

void ShaderOpenGL::CheckCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
			std::string errorMessage =
			    "ERROR::SHADER_COMPILATION_ERROR of type: " + type + "\n" + infoLog + "\n -- --------------------------------------------------- -- " + "\n";
			std::cout << errorMessage << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
			std::string errorMessage =
			    "ERROR::PROGRAM_LINKING_ERROR of type: " + type + "\n" + infoLog + "\n -- --------------------------------------------------- -- " + "\n";
			std::cout << errorMessage << std::endl;
		}
	}
}

unsigned int ShaderOpenGL::GetID() const
{
	return shader_ID;
}

// Static functions
void ShaderOpenGL::Use(unsigned int shader_ID)
{
	glUseProgram(shader_ID);
}

void ShaderOpenGL::setBool(unsigned int shader_ID, const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(shader_ID, name.c_str()), (int)value);
}

void ShaderOpenGL::setInt(unsigned int shader_ID, const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(shader_ID, name.c_str()), value);
}

void ShaderOpenGL::setFloat(unsigned int shader_ID, const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(shader_ID, name.c_str()), value);
}

void ShaderOpenGL::setVec2(unsigned int shader_ID, const std::string& name, const glm::vec2& value)
{
	glUniform2fv(glGetUniformLocation(shader_ID, name.c_str()), 1, &value[0]);
}

void ShaderOpenGL::setVec2(unsigned int shader_ID, const std::string& name, float x, float y)
{
	glUniform2f(glGetUniformLocation(shader_ID, name.c_str()), x, y);
}

void ShaderOpenGL::setVec3(unsigned int shader_ID, const std::string& name, const glm::vec3& value)
{
	glUniform3fv(glGetUniformLocation(shader_ID, name.c_str()), 1, &value[0]);
}

void ShaderOpenGL::setVec3(unsigned int shader_ID, const std::string& name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(shader_ID, name.c_str()), x, y, z);
}

void ShaderOpenGL::setVec4(unsigned int shader_ID, const std::string& name, const glm::vec4& value)
{
	glUniform4fv(glGetUniformLocation(shader_ID, name.c_str()), 1, &value[0]);
}

void ShaderOpenGL::setVec4(unsigned int shader_ID, const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(shader_ID, name.c_str()), x, y, z, w);
}

void ShaderOpenGL::setMat2(unsigned int shader_ID, const std::string& name, const glm::mat2& mat)
{
	glUniformMatrix2fv(glGetUniformLocation(shader_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderOpenGL::setMat3(unsigned int shader_ID, const std::string& name, const glm::mat3& mat)
{
	glUniformMatrix3fv(glGetUniformLocation(shader_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderOpenGL::setMat4(unsigned int shader_ID, const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(shader_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}