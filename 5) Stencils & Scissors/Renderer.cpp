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
	SetTextureRepeating(textures[1], true);//�������ظ� ���Ի���ȫ���ظ�������ͼ��

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
	| GL_STENCIL_BUFFER_BIT);//�ü���buffer �� depth buffer ������ ͨ�����ԵĲ��ֻᱻ���� û��ͨ���Ļᱻ�ü�

	if (usingScissor) {
		glEnable(GL_SCISSOR_TEST);
		glScissor((float)width / 2.5f, (float)height / 2.5f,
			(float)width / 5.0f, (float)height / 5.0f);   //Scissor��һ���ı��� bottom left x, bottom left y , width,height
	}
	BindShader(shader);
	
	textureMatrix = Matrix4::Scale(Vector3(6, 6, 6));
	UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(shader-> GetProgram(),
			"diffuseTex"), 0);

	if (usingStencil) {
		//ģ�建�������ģ�建����������Ƭ�ε�ģ��ֵΪ0��Ȼ��������Ƭ����1��䡣�����е�ģ��ֵΪ1����ЩƬ�βŻᱻ��Ⱦ�������Ķ�����������

		glEnable(GL_STENCIL_TEST);
		
		//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);//��ɫ���� �ر�����ͨ������ɫд��
		glStencilFunc(GL_ALWAYS, 2, ~0);//����OPENGL ��Զͨ������ ������ģ�建����ʲô
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);//��θ��»���    sfail dpfail dppass ��Ⱥ�ģ����Ե������������ʲô replace��refֵ�滻
		
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		meshes[1]-> Draw();//�ı��ε�ģ�建��ȫ����Ϊ2

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);//������ɫд��
		glStencilFunc(GL_EQUAL, 2, ~0);//����mask �Ⱥ�refֵ������� Ȼ�Ͱ�����mask��stencil�еĻ���ֵ������� �Ա��������
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	meshes[0]-> Draw();
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);

}

