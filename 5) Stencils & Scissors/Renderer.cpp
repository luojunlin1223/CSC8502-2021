#include "Renderer.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	meshes[0] = Mesh::GenerateTriangle();
	meshes[1] = Mesh::GenerateQuad();

	textures[0] = SOIL_load_OGL_texture(TEXTUREDIR "brick.tga",
	SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	textures[1] = SOIL_load_OGL_texture(TEXTUREDIR "star.png",
	SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (!textures[0] || !textures[1]) {
		return;
		}

	SetTextureRepeating(textures[0], true);
	SetTextureRepeating(textures[1], true);//设置了重复 所以会是全屏重复的星星图案

	shader = new Shader("TexturedVertex.glsl", "StencilFragment.glsl");
	if (!shader-> LoadSuccess()) {
		return;
		}
	usingScissor = false;
	usingStencil = false;
	init = true;

}

Renderer ::~Renderer(void) {
	delete meshes[0];
	delete meshes[1];
	glDeleteTextures(2, textures);
	delete shader;
}
void Renderer::ToggleScissor() {
	usingScissor = !usingScissor;
}

void Renderer::ToggleStencil() {
	usingStencil = !usingStencil;
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
	| GL_STENCIL_BUFFER_BIT);//裁剪的buffer 和 depth buffer 很类似 通过测试的部分会被保留 没有通过的会被裁剪

	if (usingScissor) {
		glEnable(GL_SCISSOR_TEST);
		glScissor((float)width / 2.5f, (float)height / 2.5f,
			(float)width / 5.0f, (float)height / 5.0f);   //Scissor是一个四边形 bottom left x, bottom left y , width,height
	}
	BindShader(shader);
	
	textureMatrix = Matrix4::Scale(Vector3(6, 6, 6));
	UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(shader-> GetProgram(),
			"diffuseTex"), 0);

	if (usingStencil) {
		//模板缓冲先清空模板缓冲设置所有片段的模板值为0，然后开启矩形片段用1填充。场景中的模板值为1的那些片段才会被渲染（其他的都被丢弃）。

		glEnable(GL_STENCIL_TEST);
		
		//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);//颜色遮罩 关闭所有通道的颜色写入
		glStencilFunc(GL_ALWAYS, 2, ~0);//告诉OPENGL 永远通过测试 描述了模板缓冲做什么
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);//如何更新缓冲    sfail dpfail dppass 深度和模板测试的三种情况下做什么 replace将ref值替换
		
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		meshes[1]-> Draw();//四边形的模板缓存全部改为2

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);//开启颜色写入
		glStencilFunc(GL_EQUAL, 2, ~0);//掩码mask 先和ref值做与操作 然和把掩码mask和stencil中的缓存值做与操作 对比两个结果
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	meshes[0]-> Draw();
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);

}

