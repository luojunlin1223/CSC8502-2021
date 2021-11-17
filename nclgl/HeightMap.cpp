#include "HeightMap.h"
#include <iostream>

HeightMap::HeightMap(const std::string& name) {
	int iWidth, iHeight, iChans;
	unsigned char* data = SOIL_load_image(name.c_str(),
		&iWidth, &iHeight, &iChans, 1);
	if (!data) {
		std::cout << "Heightmap can’t load file !\n";
		return;
	}
	numVertices = iWidth * iHeight;//高度图 假设每一个像素有一个顶点  所以顶点数就是高宽相乘
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;//每个grid 用两个三角形来表示 所以需要六个顶点
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	Vector3 vertexScale = Vector3(16.0f, 5.0f, 16.0f);
	Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

	for (int z = 0; z < iHeight; ++z) {//根据纹理中得数据 上下推移Y轴
		for (int x = 0; x < iWidth; ++x) {
			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	}
	SOIL_free_image_data(data);

	int i = 0;
	for (int z = 0; z < iHeight - 1; ++z) {//生成索引数组
		for (int x = 0; x < iWidth - 1; ++x) {
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;
			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}

	GenerateNormals();//生成法向量 计算每一个顶点的法向量

	GenerateTangents();//生成切线向量

	BufferData();//上传数据

	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 255.0f;// each height is a byte !
	heightmapSize.z = vertexScale.z * (iHeight - 1);

}
