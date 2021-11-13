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
	void PresentScene(); //���Ƹ߶�ͼ
	void DrawPostProcess();//ִ�к��ڴ���
	void DrawScene();//���ƺ���Ч��
	Shader * sceneShader;
	Shader * processShader;//���ں��ڴ����shader
	Camera * camera;
	Mesh * quad;
	HeightMap * heightMap;
	GLuint heightTexture;
	GLuint bufferFBO;//��һ��֡�������
	GLuint processFBO;//�ڶ���֡�������
	GLuint bufferColourTex[2];//��������
	GLuint bufferDepthTex;//��������
};

