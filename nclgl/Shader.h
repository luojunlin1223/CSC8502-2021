/******************************************************************************
Class:Shader
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:VERY simple class to encapsulate GLSL shader loading, linking,
and binding. Useful additions to this class would be overloaded functions to
replace the glUniformxx functions in external code, and possibly a map to store
uniform names and their resulting bindings. 

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "OGLRenderer.h"

enum ShaderStage {
	SHADER_VERTEX,
	SHADER_FRAGMENT,
	SHADER_GEOMETRY,
	SHADER_DOMAIN,
	SHADER_HULL,
	SHADER_MAX
};

class Shader	{
public:
	/**
	* @brief Shader的构造方法 把创建的shader 加入 vector allshaders中
	* @param vertex 顶点着色器的路径
	* @param fragment 片段着色器的路径
	* @param geometry 图形着色器的路径
	* @param domain 未知
	* @param hull 未知
	*/
	Shader(const std::string& vertex, const std::string& fragment, const std::string& geometry = "", const std::string& domain = "", const std::string& hull = "");
	~Shader(void);
	/**
	* @brief 获得当前shader对象对应的程序ID
	* @return 程序ID
	*/
	GLuint  GetProgram() { return programID;}
	/**
	* @brief 重新加载所有着色器
	* @param deleteOld 是否需要删除之前的shader和程序
	*/
	void	Reload(bool deleteOld = true);
	/**
	* @brief shader和程序是否创建成功
	* @return 是否成功
	*/
	bool	LoadSuccess() {
		return shaderValid[0] == GL_TRUE && programValid == GL_TRUE;
	}
	/**
	* @brief 重新加载所有的shader
	*/
	static void ReloadAllShaders();
	/**
	* @brief 打印编译结果
	*/
	static void	PrintCompileLog(GLuint object);
	/**
	* @brief 打印链接信息
	*/
	static void	PrintLinkLog(GLuint program);

protected:
	/**
	* @brief 删除当前的着色程序和shader对象
	*/
	void	DeleteIDs();
	/**
	* @brief 从文件加载shader内容
	* @param filename 文件地址
	* @param into 内容缓存
	* @return 是否加载成功
	*/
	bool	LoadShaderFile(const  std::string& from, std::string &into);
	/**
	* @brief 创建shader对象 从文件中读取shader内容并且编译
	* @param i 要生成的Shader对象的数量
	*/
	void	GenerateShaderObject(unsigned int i);
	/**
	* @brief 将shader中的变量名和通道绑定 还有一种办法是直接在shader中直接指定(layout = 0)。
	*/
	void	SetDefaultAttributes();
	/**
	* @brief 链接程序 确定程序是否链接成功
	*/
	void	LinkProgram();

	GLuint	programID; //着色程序的标识ID
	GLuint	objectIDs[SHADER_MAX];//各类着色器创建后的ID
	GLint	programValid;//程序是否链接成功的状态
	GLint	shaderValid[SHADER_MAX];//各类着色器编译后的结果 是否编译成功

	std::string  shaderFiles[SHADER_MAX]; //存放着各类着色器的文件地址

	static std::vector<Shader*> allShaders;
};

