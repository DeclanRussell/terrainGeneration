#version 400
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 texCoord;


out vec4 normal;
out float vertHeight;
out float vertNormalY;
out vec2 vertTexCoord;


uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;


void main(void)
{
    vertHeight = vertexPosition.y;
    vertNormalY = vertexNormal.y;
    vertTexCoord = texCoord;
    normal = vec4(vertexNormal,0.0);
    gl_Position = vec4(vertexPosition.x,vertexPosition.y+0.025,vertexPosition.z,1.0);
}
