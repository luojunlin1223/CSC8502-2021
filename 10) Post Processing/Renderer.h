#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);
	
	void RenderScene() override;
	void UpdateScene(float dt) override;
protected:
	void PresentScene(); //绘制高度图
	void DrawPostProcess();//执行后期处理
	void DrawScene();//绘制后期效果
	Shader * sceneShader;
	Shader * processShader;//用于后期处理的shader
	Camera * camera;
	Mesh * quad;
	HeightMap * heightMap;
	GLuint heightTexture;
	GLuint bufferFBO;//第一个帧缓冲对象
	GLuint processFBO;//第二个帧缓冲对象
	GLuint bufferColourTex[2];//附件纹理
	GLuint bufferDepthTex;//附件纹理
};

