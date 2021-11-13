#include "Renderer.h"
Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	meshes[0] = Mesh::GenerateQuad();
	meshes[1] = Mesh::GenerateTriangle();

	textures[0] = SOIL_load_OGL_texture(
		TEXTUREDIR "brick.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[1] = SOIL_load_OGL_texture(
		TEXTUREDIR "stainedglass.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (!textures[0] || !textures[1]) {
		return;
	}
	positions[0] = Vector3(0, 0, -5);
	positions[1] = Vector3(0, 0, -5);
	shader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");

	if (!shader-> LoadSuccess()) {
		return;
	}

	usingDepth = false;
	usingAlpha = false;
	blendMode = 0;
	modifyObject = true;

	projMatrix = Matrix4::Perspective(1.0f, 100.0f,
		(float)width / (float)height, 45.0f);
	init = true;
}
Renderer ::~Renderer(void) {
	delete meshes[0];
	delete meshes[1];
	delete shader;
	glDeleteTextures(2, textures);
}
void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader-> GetProgram(),
		"diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	for (unsigned int i = 0; i < 2; ++i) {
		glUniformMatrix4fv(glGetUniformLocation(
			shader-> GetProgram(), "modelMatrix"), 1, false,
			(float*)&Matrix4::Translation(positions[i]));
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		meshes[i]-> Draw();
	}
}

void Renderer::ToggleObject() {
	modifyObject = !modifyObject;
}

void Renderer::MoveObject(float by) {
	positions[(int)modifyObject].z += by;
}

void Renderer::ToggleBlendMode() {
	blendMode = (blendMode + 1) % 4;
	switch (blendMode) {
		//把某一像素位置原来的颜色和将要画上去的颜色，通过某种方式混在一起，从而实现特殊的效果
		//透过红色的玻璃去看绿色的物体，那么可以先绘制绿色的物体，再绘制红色玻璃。
		//在绘制红色玻璃的时候，利用“混合”功能，把将要绘制上去的红色和原来的绿色进行混合，于是得到一种新的颜色，看上去就好像玻璃是半透明的。

		case (0): glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;//颜色混合函数 Sfactor源因子 dfactor目标因子 
		//源颜色乘以自身的alpha 值，目标颜色乘以1.0减去源颜色的alpha值，这样一来，源颜色的alpha值越大，则产生的新颜色中源颜色所占比例就越大，而目标颜色所占比例则减 小。
		//这种情况下，我们可以简单的将源颜色的alpha值理解为“不透明度”。这也是混合时最常用的方式。
		case (1): glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR); break;
		case (2): glBlendFunc(GL_ONE, GL_ZERO); break;
		case (3): glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
	}
}

void Renderer::ToggleDepth() {
	usingDepth = !usingDepth;
	usingDepth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void Renderer::ToggleAlphaBlend() {
	usingAlpha = !usingAlpha;
	usingAlpha ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}
