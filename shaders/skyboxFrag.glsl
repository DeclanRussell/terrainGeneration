#version 400

uniform samplerCube cubeMapTex;
uniform sampler2D cloudTex;
uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
uniform float time;

in vec3 viewDir;
in vec2 TexCoords;
in vec3 position;
out vec4 fragColour;
in vec3 pos;

vec3 calcFog(vec3 texColour){
  float dist = length(viewDir.xyz);
  vec3 rayDir = normalize(-viewDir.xyz);
  vec3 sunDir = vec3(0.0, 0.3, 1.0);
  float sunAmount = max(dot( rayDir, sunDir ), 0.0 );
  sunAmount *= 1.1;
  vec3  fogColour  = mix(texColour, vec3(1.0,0.9, 0.7), pow(sunAmount,8.0) );
  return fogColour;

}

vec3 clouds(){
  float o = float(time) * 0.000005;
  vec2  Offset = vec2(-o, o);

  vec4 skyColour = vec4(136.0/255.0, 151.0/255.0, 186.0/255.0, 1.0);
  vec4 cloudColour = vec4(1.0, 1.0, 1.0, 1.0);

  vec4 noise = texture(cloudTex, TexCoords + Offset);
  float t = (cos( noise.g * 3.14 ) + 1.0) / 2.0;
  vec4 colour = mix(skyColour, cloudColour, t);
  return colour.rgb;
}

void  main(){
   vec3 cloudsColour = clouds();
   vec3 colour = calcFog(cloudsColour);
   fragColour = vec4(colour, 1.0);
}
