#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Use glm::vec4
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

private:
	std::unordered_map<std::string, unsigned int> mUniformLocationCache;
	std::string mFilePath;
	unsigned int mRendererID;
	unsigned int GetUniformLocation(const std::string& name);
	ShaderProgramSource ParseShader(const std::string & filepath);
	unsigned int CompileShader(unsigned int type, const std::string & source);
	unsigned int CreateShader(const std::string & vertexShader, const std::string & fragmentShader);
};


#endif