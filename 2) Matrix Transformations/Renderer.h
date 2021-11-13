#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
class Renderer:public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);
	virtual void RenderScene();
	/**
	* @brief 转换到投影矩阵
	*/
	void SwitchToPerspective();
	/**
	* @brief 转换到正交矩阵
	*/
	void SwitchToOrthographic();
	virtual void UpdateScene(float dt);
	inline void SetScale(float s) { scale = s;}
	inline void SetRotation(float r) { rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; }

protected:
	Camera* camera;//相机对象
	Mesh* triangle;
	Shader* matrixShader;
	float scale;//缩放大小
	float rotation;//旋转角度
	Vector3 position;//位置
};

