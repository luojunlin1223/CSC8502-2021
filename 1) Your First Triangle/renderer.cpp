#include "renderer.h"
Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
 triangle = Mesh::GenerateTriangle();

 basicShader = new Shader("basicVertex.glsl","colourFragment.glsl");

 if (!basicShader->LoadSuccess()){
 return;
}
 init = true;//加载shader程序 初始化成功
}
Renderer ::~Renderer(void) {
 delete triangle;
 delete basicShader;
}

void Renderer::RenderScene() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);//清屏颜色
	glClear(GL_COLOR_BUFFER_BIT);//清理上一帧的颜色
	BindShader(basicShader);
	triangle->Draw();
}
