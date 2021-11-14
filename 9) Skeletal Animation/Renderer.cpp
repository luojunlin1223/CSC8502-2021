
#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
	(float)width / (float)height, 45.0f);
	camera = new Camera(-3, 0.0f, Vector3(0, 1.4f, 4.0f));

	shader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");

	if (!shader-> LoadSuccess()) {
		return;
	}

	mesh = Mesh::LoadFromMeshFile("Role_T.msh");
	anim = new MeshAnimation("Role_T.anm");
	material = new MeshMaterial("Role_T.mat");

	for (int i = 0; i < mesh-> GetSubMeshCount(); ++i) {//例子有5个mesh
		const MeshMaterialEntry * matEntry =material-> GetMaterialForLayer(i);//RealisticCharacter/Role/Hair.tag 
	
		const string * filename = nullptr;
		matEntry-> GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;         //+		path	"../Textures//RealisticCharacter/Role/Hair.tga"	std::string
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);   //使用了多个纹理贴图 对应不同的mesh
	}
	currentFrame = 0;
	frameTime = 0.0f;
	init = true;
}

Renderer ::~Renderer(void) {
	delete camera;
	delete mesh;
	delete anim;
	delete material;
	delete shader;
}

void Renderer::UpdateScene(float dt) {
	camera-> UpdateCamera(dt);
	viewMatrix = camera -> BuildViewMatrix();
	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) %anim-> GetFrameCount(); //根据动画时间来更新
		frameTime += 1.0f /anim-> GetFrameRate();
	}
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(shader);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);

	UpdateShaderMatrices();

	vector < Matrix4 > frameMatrices;
	const Matrix4* invBindPose = mesh->GetInverseBindPose();      

	//根据帧获得对应的joint数据，同时要和inversebindpose相乘 解除BindPose
	// 
	//如果有一个骨架 没有动画运用给它 它一般都会呈T字形状 这是为了让它更简单

	//在建模时访问单个顶点，并将网格置于完全“拉伸”状态，

	//使其更容易权重每个顶点到正确的关节。

	const Matrix4* frameData = anim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < mesh-> GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}

	int j = glGetUniformLocation(shader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false,
		(float*)frameMatrices.data());

	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		mesh->DrawSubMesh(i);
	}
}


