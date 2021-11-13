#version 330 core
in vec3 position;//在函数中已经用名字绑定了通道数
in vec4 colour;

out Vertex{
	vec4 colour;
} OUT;

void main(void){
	gl_Position = vec4(position, 1.0);//顶点着色器一定要输出的内容
	OUT.colour = colour;
}