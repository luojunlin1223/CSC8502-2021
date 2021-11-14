#include "Renderer.h"
const int POST_PASSES = 10;
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	//把渲染出来的帧不输入backbuffer而是输入到一个texture中
	//texture可以操作周围的采样而不是去移动那些顶点
	//把所有的图形都渲染到一个texture之后 创建一个四边形 如果四边形的大小合适 贴上贴图就会显示之前的内容
	//渲染两个三角形铺满屏幕会产生浪费，解决方案是用一个很大的三角形（超出屏幕）来铺满整个屏幕
	//在fragment shader里面可以更改传来的纹理颜色 来达到一些效果

	//做一个比屏幕大的纹理 然后使用模糊 就可以避免屏幕边缘没有像素的问题

	//一个高度图和一个四边形

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
	} //创建组件 以及两个着色器

	SetTextureRepeating(heightTexture, true);
	// Generate our scene depth texture ...
	
	//我们需要生成三个纹理 一个深度纹理 两个颜色纹理
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);  //深度纹理的生成
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);//32位的颜色附件，8位RGB和一个Channel 
	//24位的Z buffer    
	// And our colour texture ...两个颜色纹理
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
	glGenFramebuffers(1, &bufferFBO); // We ’ll render the scene into this
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//解除绑定
	glEnable(GL_DEPTH_TEST);//开启深度测试
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
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);//绑定FBO对象
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | //清除上一帧的深度颜色和模板缓存
		GL_STENCIL_BUFFER_BIT);
	//将高度图添加到它的第一个颜色附件sceneColourTex[0]
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

void Renderer::DrawPostProcess() {//我们将使用另一个FBO，但是，不是渲染高度图，
	//我们将渲染一个单一屏幕大小的四边形，使用一个特殊的后期处理碎片着色器

	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_2D, bufferColourTex[1], 0);//0是指定要附加的纹理图像的mipmap的级别，必须为0
	//可以将ColorBuffer绑定到一张Texture上。当然也可以绑定到多张贴图中。另外使用这个函数也可以取出深度贴图。
	//当生成了自己的fbo之后，即可将图元绘制到贴图上面，可以理解为是一个预渲染的过程。
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);//

	BindShader(processShader);
	modelMatrix.ToIdentity();

	//确保我们的矩阵不会引起任何不必要的转换，通过设置它们为单位矩阵。

	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);//激活一个绑定点
	glUniform1i(glGetUniformLocation(
		processShader-> GetProgram(), "sceneTex"), 0);//绑定到激活的绑定点
	 for (int i = 0; i < POST_PASSES; ++i) {
		 //ping pong 过程 就是把一个纹理处理后又存储在一个纹理中，继续后续操作

		//POST_PASSES 定义了处理的次数 在这里会使用高斯模糊10次
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[1], 0);//颜色附件
		glUniform1i(glGetUniformLocation(processShader-> GetProgram(),
			"isVertical"), 0);//isVertical 赋值

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
