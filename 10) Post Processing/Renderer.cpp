#include "Renderer.h"
const int POST_PASSES = 10;
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	//����Ⱦ������֡������backbuffer�������뵽һ��texture��
	//texture���Բ�����Χ�Ĳ���������ȥ�ƶ���Щ����
	//�����е�ͼ�ζ���Ⱦ��һ��texture֮�� ����һ���ı��� ����ı��εĴ�С���� ������ͼ�ͻ���ʾ֮ǰ������
	//��Ⱦ����������������Ļ������˷ѣ������������һ���ܴ�������Σ�������Ļ��������������Ļ
	//��fragment shader������Ը��Ĵ�����������ɫ ���ﵽһЩЧ��

	//��һ������Ļ������� Ȼ��ʹ��ģ�� �Ϳ��Ա�����Ļ��Եû�����ص�����

	//һ���߶�ͼ��һ���ı���

	camera = new Camera(-25.0f, 225.0f,Vector3(-150.0f, 250.0f, -150.0f));

	quad = Mesh::GenerateQuad();

	heightMap = new HeightMap(TEXTUREDIR "noise.png");
	heightTexture =SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	sceneShader = new Shader("TexturedVertex.glsl",
			"TexturedFragment.glsl");
	processShader = new Shader("TexturedVertex.glsl",
		 "processfrag.glsl");
		if (!processShader-> LoadSuccess() || !sceneShader-> LoadSuccess() ||
			!heightTexture) {
		return;
	} //������� �Լ�������ɫ��

	SetTextureRepeating(heightTexture, true);
	// Generate our scene depth texture ...
	
	//������Ҫ������������ һ��������� ������ɫ����
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);  //������������
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);//32λ����ɫ������8λRGB��һ��Channel 
	//24λ��Z buffer    
	// And our colour texture ...������ɫ����
	 for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	glGenFramebuffers(1, &bufferFBO); // We ��ll render the scene into this
	glGenFramebuffers(1, &processFBO); // And do post processing in this
	
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[0], 0);
	 // We can check FBO attachment success using this command !
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
			GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
		 return;		
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//�����
	glEnable(GL_DEPTH_TEST);//������Ȳ���
	init = true;
}
Renderer ::~Renderer(void) {
	delete sceneShader;
	delete processShader;
	delete heightMap;
	delete quad;
	delete camera;
	
	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
}

void Renderer::UpdateScene(float dt) {
	camera-> UpdateCamera(dt);
	viewMatrix = camera-> BuildViewMatrix();
	
}

void Renderer::RenderScene() {
	DrawScene();
	DrawPostProcess();
	PresentScene();
}
void Renderer::DrawScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);//��FBO����
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | //�����һ֡�������ɫ��ģ�建��
		GL_STENCIL_BUFFER_BIT);
	//���߶�ͼ��ӵ����ĵ�һ����ɫ����sceneColourTex[0]
	BindShader(sceneShader);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		 (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(
		sceneShader-> GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightTexture);
	heightMap-> Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPostProcess() {//���ǽ�ʹ����һ��FBO�����ǣ�������Ⱦ�߶�ͼ��
	//���ǽ���Ⱦһ����һ��Ļ��С���ı��Σ�ʹ��һ������ĺ��ڴ�����Ƭ��ɫ��

	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_2D, bufferColourTex[1], 0);//0��ָ��Ҫ���ӵ�����ͼ���mipmap�ļ��𣬱���Ϊ0
	//���Խ�ColorBuffer�󶨵�һ��Texture�ϡ���ȻҲ���԰󶨵�������ͼ�С�����ʹ���������Ҳ����ȡ�������ͼ��
	//���������Լ���fbo֮�󣬼��ɽ�ͼԪ���Ƶ���ͼ���棬�������Ϊ��һ��Ԥ��Ⱦ�Ĺ��̡�
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);//

	BindShader(processShader);
	modelMatrix.ToIdentity();

	//ȷ�����ǵľ��󲻻������κβ���Ҫ��ת����ͨ����������Ϊ��λ����

	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);//����һ���󶨵�
	glUniform1i(glGetUniformLocation(
		processShader-> GetProgram(), "sceneTex"), 0);//�󶨵�����İ󶨵�
	 for (int i = 0; i < POST_PASSES; ++i) {
		 //ping pong ���� ���ǰ�һ����������ִ洢��һ�������У�������������

		//POST_PASSES �����˴���Ĵ��� �������ʹ�ø�˹ģ��10��
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[1], 0);//��ɫ����
		glUniform1i(glGetUniformLocation(processShader-> GetProgram(),
			"isVertical"), 0);//isVertical ��ֵ

		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		quad-> Draw();
		// Now to swap the colour buffers , and do the second blur pass
		glUniform1i(glGetUniformLocation(processShader-> GetProgram(),
				 "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad-> Draw();
		
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}
void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(
		sceneShader-> GetProgram(), "diffuseTex"), 0);
	quad-> Draw();

}
