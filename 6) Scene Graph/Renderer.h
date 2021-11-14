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
	* @brief �ݹ黭���нڵ�
	*/
	void DrawNode(SceneNode * n);
	SceneNode * root;//����ĸ��ڵ�
	Camera * camera;
	Mesh * cube;
	Shader * shader;
};

