#version 330 core
#extension GL_ARB_shading_language_420pack : enable

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 pvmMatrix;
uniform mat3 normalMatrix;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 textureCoordinate;

out vec2 texCoord;

void main()
{
    gl_Position = pvmMatrix * vec4(position, 1.0);
    
	texCoord = textureCoordinate;
}
