#version 400
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;


out vec4 normal;
out float vertHeight;
out float vertNormalY;



uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;


void main(void)
{
    vertHeight = vertexPosition.y;
    vertNormalY = vertexNormal.y;
    normal = vec4(vertexNormal,0.0);
    gl_Position = vec4(vertexPosition,1.0);
}
