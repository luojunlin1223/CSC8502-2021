/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Wrapper around OpenGL primitives, geometry and related 
OGL functions.

There's a couple of extra functions in here that you didn't get in the tutorial
series, to draw debug normals and tangents. 


-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OGLRenderer.h"
#include <vector>
#include <string>

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
enum MeshBuffer {//枚举 创建的时候 用max 再分别把顶点颜色等等属性 分别放入数据的对应位置
	VERTEX_BUFFER	,
	COLOUR_BUFFER	, 
	TEXTURE_BUFFER	,
	NORMAL_BUFFER	, 
	TANGENT_BUFFER	,

	WEIGHTVALUE_BUFFER,		//new this year, weight values of vertices
	WEIGHTINDEX_BUFFER,	//new this year, indices of weights

	INDEX_BUFFER	,

	MAX_BUFFER
};

class Mesh	{
public:	
	struct SubMesh {
		int start;
		int count;
	};

	Mesh(void);
	~Mesh(void);
	/**
	* @brief 绘制网格体 可以选择是否使用EBO绘制
	*/
	void Draw();
	void DrawSubMesh(int i);
	/**
	* @brief 从mesh文件加载内容
	* @param name 文件地址名
	* @return 网格体
	*/
	static Mesh* LoadFromMeshFile(const std::string& name);

	unsigned int GetTriCount() const {
		int primCount = indices ? numIndices : numVertices;
		return primCount / 3;
	}

	unsigned int GetJointCount() const {
		return (unsigned int)jointNames.size();
	}


	int GetIndexForJoint(const std::string& name) const;
	int GetParentForJoint(const std::string& name) const;
	int GetParentForJoint(int i) const;

	const Matrix4* GetBindPose() const {
		return bindPose;
	}

	const Matrix4* GetInverseBindPose() const {
		return inverseBindPose;
	}

	int		GetSubMeshCount() const {
		return (int)meshLayers.size(); 
	}

	bool GetSubMesh(int i, const SubMesh* s) const;
	bool GetSubMesh(const std::string& name, const SubMesh* s) const;

	/**
	* @brief 生成一个三角形网格体把顶点颜色贴图数据录入并且上传给GPU
	* @return 三角形的网格体
	*/
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();
	/**
	* @brief 计算每一个顶点的法向量
	*/
	void GenerateNormals();
	/**
	* @brief 从index中得到三角形的顶点信息
	* @param i 哪一个三角形
	* @param abc 接受indices
	*/
	bool GetVertexIndicesForTri(unsigned int i,
		unsigned int& a, unsigned int& b, unsigned int& c) const;
	/**
	* @brief 生成切线向量
	*/
	void GenerateTangents();
	/**
	* @brief 计算切线
	*/
	Vector4 GenerateTangent(int a, int b, int c);


protected:
	void	BufferData();	

	GLuint	arrayObject;//VAO Unsigned int 类型

	GLuint	bufferObject[MAX_BUFFER];//VBO

	GLuint	numVertices;//顶点数量

	GLuint	numIndices;//EBO
	
	GLuint	type;

	Vector3*		vertices;
	Vector4*		colours;
	Vector2*		textureCoords;
	Vector3*		normals;
	Vector4*		tangents;

	Vector4*		weights;
	int*			weightIndices;

	unsigned int*	indices;

	Matrix4* bindPose;
	Matrix4* inverseBindPose;

	std::vector<std::string>	jointNames;
	std::vector<int>			jointParents;
	std::vector< SubMesh>		meshLayers;
	std::vector<std::string>	layerNames;
};

