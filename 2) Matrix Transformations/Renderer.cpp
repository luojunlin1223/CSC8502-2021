#include "Renderer.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();

	matrixShader = new Shader("MatrixVertex.glsl","colourFragment.glsl");

	if (!matrixShader->LoadSuccess()) {
		return;
	}

	init = true;

	SwitchToOrthographic();
	camera = new Camera();
}

Renderer ::~Renderer(void) {
	delete triangle;
	delete matrixShader;
	delete camera;
}

void Renderer::SwitchToPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
	(float)width / (float)height, 45.0f);
}
void Renderer::SwitchToOrthographic() {
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,
	width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}
void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(matrixShader);

	glUniformMatrix4fv(glGetUniformLocation(matrixShader-> GetProgram()
		, "projMatrix"), 1, false, projMatrix.values);//向shader的unifrom变量传递内容 第二个参数是count 
	//如果目标的uniform变量不是数组，则应该是1，如果它是数组，则应该是1或更多。

	glUniformMatrix4fv(glGetUniformLocation(matrixShader-> GetProgram()
		, "viewMatrix"), 1, false, viewMatrix.values);

	for (int i = 0; i < 3; ++i) {
		Vector3 tempPos = position;
		tempPos.z += (i * 500.0f);
		tempPos.x -= (i * 100.0f);
		tempPos.y -= (i * 100.0f);

	modelMatrix = Matrix4::Translation(tempPos) *
	Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
	 Matrix4::Scale(Vector3(scale, scale, scale)); //通常的变化顺序是 SRT 相乘的时候需要把顺序进行调换

	glUniformMatrix4fv(glGetUniformLocation(
	matrixShader-> GetProgram(), "modelMatrix"),
	1, false, modelMatrix.values);
	triangle-> Draw();
	}
}
void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}
