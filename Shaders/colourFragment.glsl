#version 330 core

in Vertex{
	vec4 colour;
} IN;


out vec4 fragColour;//片段着色器一定要输出的数据
void main(void) {
	fragColour = IN.colour;
}