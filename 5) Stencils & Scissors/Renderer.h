#pragma once
#include "../nclgl/OGLRenderer.h"
class Renderer:public OGLRenderer
{
public:
	Renderer(Window & parent);
	~Renderer(void);
	
	void RenderScene() override;
	
	void ToggleScissor();
	void ToggleStencil();
	
protected:
	Shader * shader;
	Mesh * meshes[2];//一个三角形和一个四边形
	GLuint textures[2];
	
	bool usingScissor;
	bool usingStencil;
};

