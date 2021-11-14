#pragma once
#include "..//nclgl/Camera.h"
#include "../nclgl/OGLRenderer.h"
#include "..//nclgl/SceneNode.h"
#include "..//nclgl/CubeRobot.h"
class Renderer:public OGLRenderer
{
public:
	Renderer(Window & parent);
	~Renderer(void);
	
	void UpdateScene(float dt) override;
	void RenderScene() override;
protected:
	/**
	* @brief 递归画所有节点
	*/
	void DrawNode(SceneNode * n);
	SceneNode * root;//物体的根节点
	Camera * camera;
	Mesh * cube;
	Shader * shader;
};

