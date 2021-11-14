#pragma once
/*
Class:OGLRenderer
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL 
3.2+ CORE PROFILE rendering context. Each lesson will create a renderer that 
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#include "Common.h"

#include <string>
#include <fstream>
#include <vector>

#include "KHR\khrplatform.h"
#include "glad\glad.h"

#include "GL/GL.h"
#include "KHR/WGLext.h"

#include "SOIL/SOIL.h"

#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "Window.h"
#include "Shader.h"
#include "Mesh.h"

using std::vector;

#define OPENGL_DEBUGGING

extern const Matrix4 biasMatrix;

class Shader;
class Light;

class OGLRenderer	{
public:
	friend class Window;
	OGLRenderer(Window &parent);
	virtual ~OGLRenderer(void);
	/**
	* @brief 渲染过程
	*/
	virtual void	RenderScene()		= 0;
	/**
	* @brief 更新渲染过程
	*/
	virtual void	UpdateScene(float msec);
	/**
	* @brief 交换前后缓存 需要在RenderScene后调用
	*/
	void			SwapBuffers();
	/**
	* @brief 检查所有的构造器是否按照计划进行
	* @return 初始化状态
	*/
	bool			HasInitialised() const;	
	
protected:
	virtual void	Resize(int x, int y);
	/**
	* @brief 更新当前shader中的所有矩阵
	*/
	void			UpdateShaderMatrices();
	/**
	* @brief 指定使用某一个程序 着色器对象有方法可以获得对应的程序ID
	* @param 着色器对象
	*/
	void			BindShader(Shader*s);

	void SetTextureRepeating(GLuint target, bool state);

	void StartDebugGroup(const std::string& s) {
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, (GLsizei)s.length(), s.c_str());
	}

	void EndDebugGroup() {
		glPopDebugGroup();
	}

	Matrix4 projMatrix;		//Projection matrix 投影矩阵
	Matrix4 modelMatrix;	//Model matrix. NOT MODELVIEW 从本地坐标转换到世界坐标 
	Matrix4 viewMatrix;		//View matrix  相机视口的矩阵
	Matrix4 textureMatrix;	//Texture matrix
	Matrix4 shadowMatrix;

	int		width;			//Render area width (not quite the same as window width)
	int		height;			//Render area height (not quite the same as window height)
	bool	init;			//Did the renderer initialise properly?
	/**
	* @brief 在fragment shader中设置了 光源的位置 颜色 以及辐射范围
	*/
	void SetShaderLight(const Light& l);

private:
	Shader* currentShader;	//当前启用的shader
	HDC		deviceContext;	//...Device context?
	HGLRC	renderContext;	//Permanent Rendering Context
#ifdef _DEBUG
	static void CALLBACK DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif
};