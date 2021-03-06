#include "Renderer.h"
#include "../nclgl/camera.h"
#include "../nclgl/HeightMap.h"
Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	heightMap = new HeightMap(TEXTUREDIR "noise.png");
	camera = new Camera(-40, 270, Vector3());

	Vector3 dimensions = heightMap->GetHeightmapSize();
	camera->SetPosition(dimensions * Vector3(0.5, 2, 0.5)); //高度图得高度是一个byte X和Y是缩放后得大小
	shader = new Shader("TerrainVertex.glsl ",
		"TerrainFragment.glsl");
	if (!shader->LoadSuccess()) {
		return;
	}

	terrainTex_green = SOIL_load_OGL_texture(TEXTUREDIR "Green.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainTex_white= SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!terrainTex_green||!terrainTex_white) {
		return;

	}

	SetTextureRepeating(terrainTex_green, true);
	SetTextureRepeating(terrainTex_white, true);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);//不显示地形的背面
	init = true;
}
Renderer ::~Renderer(void) {
	delete heightMap;
	delete camera;
	delete shader;
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}
void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"anotherTex"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTex_green);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrainTex_white);


	heightMap->Draw();
}
