#version 400
layout (location = 0) in vec3 vertexPosition;

out float vertHeight;

uniform sampler2D geoTexture;

uniform vec4 map_position;
uniform vec4 offset;
uniform vec4 scale;
uniform mat4 projectionMatrix;
uniform float heightScale;

void main(void)
{
    vec4 pos = scale * (offset+vec4(vertexPosition, 1.0));
    vec4 pos_map = ((pos+map_position) * 0.125)+0.5;
    pos_map /= 4.0;
    pos_map+= 0.5;
    pos.y = float(texture(geoTexture, pos_map.xz).r + texture(geoTexture, pos_map.xz).g + texture(geoTexture, pos_map.xz).b);
    pos.y/=3.0;
    vertHeight = pos.y;
    pos.y*=heightScale;
    gl_Position = pos;
}
