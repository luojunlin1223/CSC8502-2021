#include "Renderer.h"
Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();
	texture = SOIL_load_OGL_texture(TEXTUREDIR "brick.tga",
	SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);//从文件加载贴图资源 获得index
	if (!texture) {
		return;
	}
	shader = new Shader("TexturedVertex.glsl", "texturedfragment.glsl");
	if (!shader->LoadSuccess()) {
		return;
	}
	filtering = true;
	repeating = false;
	init = true;
}
Renderer ::~Renderer(void) {
	delete triangle;
	delete shader;
	glDeleteTextures(1, &texture);
}
void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0); //有16个 默认是启动的第零个  这意味着可以绑定多个贴图
	glBindTexture(GL_TEXTURE_2D, texture);
	triangle-> Draw();
}

void Renderer::UpdateTextureMatrix(float value) {
	Matrix4 push = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 pop = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pop * rotation * push;//如果不加push和pop 贴图的旋转会以左上角 0，0处为零点
}
void Renderer::ToggleRepeating() {
	repeating = !repeating;
	SetTextureRepeating(texture, repeating);
}
void Renderer::ToggleFiltering() {
	filtering = !filtering;
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);    //临近取值还是线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}