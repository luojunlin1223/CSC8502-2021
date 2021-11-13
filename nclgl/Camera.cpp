#include "Camera.h"
#include "Window.h"
#include <algorithm>
void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);//鼠标向上得到的relativeposition是负的 但是仰角应该向上 所以取负
	yaw -= (Window::GetMouse()->GetRelativePosition().x);//同理 因为opengl是右手系   
	//向上的方向是pitch的正方向 向左是yaw的正方向

	pitch = std::min(pitch, 90.0f);//俯仰角的最大最小只能往上看或者往下看
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {//左右视野可以看到360度
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));//围绕Y轴旋转
	Vector3 forward = rotation * Vector3(0, 0, -1); //cross product 指向前方的方向向量
	Vector3 right = rotation * Vector3(1, 0, 0);//指向右方方向向量
	float speed = 30.0f * dt;
	if (Window::GetKeyboard()-> KeyDown(KEYBOARD_W)) {
		position += forward * speed;
	}
	if (Window::GetKeyboard()-> KeyDown(KEYBOARD_S)) {
		position -= forward * speed;
	}
	if (Window::GetKeyboard()-> KeyDown(KEYBOARD_A)) {
		position -= right * speed;
	}
	if (Window::GetKeyboard()-> KeyDown(KEYBOARD_D)) {
		position += right * speed;
	}
	if (Window::GetKeyboard()-> KeyDown(KEYBOARD_SHIFT)) {
		 position.y += speed;
	}
	if (Window::GetKeyboard()-> KeyDown(KEYBOARD_SPACE)) {
		position.y -= speed;
	}
}
	Matrix4 Camera::BuildViewMatrix() {
		return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);   //相机的逆向变换 SRT-->>>TRS 全部需要取负 想象相机被至于了世界坐标的中心 View相对于一个逆矩阵
};
