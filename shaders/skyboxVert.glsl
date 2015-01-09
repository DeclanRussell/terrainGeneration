  #version 400
layout (location = 0) in vec3 vertexPosition;
layout (location = 2) in vec2 texCoords;

out vec3 viewDir;
out vec3 position;
out vec3 pos;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;




uniform vec3 LightPos;
uniform float Scale;

out vec2 TexCoords;

void main(){
  TexCoords = texCoords;
    viewDir = vertexPosition;
    pos = vertexPosition;
    position = vec3(modelViewMatrix * vec4(vertexPosition, 1.0));
    gl_Position = modelViewProjectionMatrix * vec4(vertexPosition,1.0);

}
