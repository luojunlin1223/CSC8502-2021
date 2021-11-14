#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>

class SceneNode {
public:
	/**
	* @brief ����һ������ڵ� Ĭ����ɫΪ1 1 1 1
	*/
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	void AddChild(SceneNode* s);
	/**
	* @brief �ݹ����ÿһ����������� ���е��ӽڵ㶼Ҫ���Ը��ڵ����������
	*/
	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer& r);

	std::vector <SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}
	std::vector <SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}
protected:
	SceneNode* parent; //���ڵ�
	Mesh* mesh; //��ǰ�ڵ��������
	Matrix4 worldTransform;//��ǰ�ڵ����������
	Matrix4 transform;//��ǰ�ڵ�ı任����
	Vector3 modelScale;//��ǰ�ڵ�Ĵ�С
	Vector4 colour;//��ǰ�ڵ����ɫ
	std::vector <SceneNode*> children;//�ӽڵ�
};

