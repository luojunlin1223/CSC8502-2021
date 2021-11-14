#pragma once
#include"..//nclgl/SceneNode.h"
class CubeRobot:public SceneNode
{
public:
	/**
	* @brief 创建一个机器人
	* @param cube 构建机器人使用的基本方块网格体
	*/
	CubeRobot(Mesh * cube);
	~CubeRobot(void) {};
	void Update(float dt) override;
	
protected:
	SceneNode * head;
	SceneNode * leftArm;
	SceneNode * rightArm;
};

