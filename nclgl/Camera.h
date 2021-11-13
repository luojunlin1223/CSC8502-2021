#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Camera
{
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
	};
	Camera(float pitch, float yaw, Vector3 position) {
		this-> pitch = pitch;
		this-> yaw = yaw;
		this-> position = position;

	}
	~Camera(void) {};
	/**
	* @brief 更新相机的各项数据 包括俯仰角 位置等等
	* @param dt 单位改变的步长
	*/
	void UpdateCamera(float dt = 1.0f);
	/**
	* @brief 组合相机移动数据 构建viewmatrix
	*/
	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }

	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }

protected:
	float yaw;//偏摆
	float pitch;//俯仰
	//roll 翻滚
	Vector3 position; // Set to 0 ,0 ,0 by Vector3 constructor ;)
};

