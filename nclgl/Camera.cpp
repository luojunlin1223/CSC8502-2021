#include "Camera.h"
#include "Window.h"
#include <algorithm>
void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);//������ϵõ���relativeposition�Ǹ��� ��������Ӧ������ ����ȡ��
	yaw -= (Window::GetMouse()->GetRelativePosition().x);//ͬ�� ��Ϊopengl������ϵ   
	//���ϵķ�����pitch�������� ������yaw��������

	pitch = std::min(pitch, 90.0f);//�����ǵ������Сֻ�����Ͽ��������¿�
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {//������Ұ���Կ���360��
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));//Χ��Y����ת
	Vector3 forward = rotation * Vector3(0, 0, -1); //cross product ָ��ǰ���ķ�������
	Vector3 right = rotation * Vector3(1, 0, 0);//ָ���ҷ���������
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
		Matrix4::Translation(-position);   //���������任 SRT-->>>TRS ȫ����Ҫȡ�� ���������������������������� View�����һ�������
};
