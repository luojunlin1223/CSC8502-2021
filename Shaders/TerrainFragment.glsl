#version 330 core

uniform sampler2D diffuseTex ;//贴图用的是sampler2D 一个着色器可以使用多于一个纹理
uniform sampler2D anotherTex;

in Vertex {
vec2 texCoord ;
vec3 v_position;
} IN ;

out vec4 fragColour ;
void main ( void ) {
float alpha=IN.v_position.y/140;
fragColour = texture ( diffuseTex , IN.texCoord )*(1-alpha)+texture(anotherTex, IN.texCoord)*alpha;
}
