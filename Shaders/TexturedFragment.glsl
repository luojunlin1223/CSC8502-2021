#version 330 core

uniform sampler2D diffuseTex ;//贴图用的是sampler2D 一个着色器可以使用多于一个纹理

in Vertex {
vec2 texCoord ;
} IN ;

out vec4 fragColour ;
void main ( void ) {
fragColour = texture ( diffuseTex , IN.texCoord );
}
