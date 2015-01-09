#version 400

in vec4 position;
in vec4 fragNorm;
in vec2 texCoord;


struct lightInfo{
   vec4 position;
   vec3 intensity;
};

uniform lightInfo light;
uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform float shininess;

uniform float fogMax;
uniform float fogMin;


vec3 ads(){
   vec3 n = normalize(vec3(fragNorm));
   vec3 s = normalize(vec3(light.position) - vec3(position));
   vec3 v = normalize(vec3(-position));
   vec3 r = reflect(-s, n);
   vec3 h = normalize(v + s);
   return light.intensity * (Ka + Kd * max(dot(s,n),0.0)+ Ks * pow(max(dot(h, n), 0.0), shininess));
}

out vec4 fragColour;
uniform sampler2D grassTexture;

vec4 calcFog(vec4 texColour){
  float dist = length(position.xyz);
  vec3 rayDir = normalize(-position.xyz);
  vec3 sunDir = vec3(0.0, -0.3, -1.0);
  float fogAmount = exp( 1.0 -dist*1.0 );
  float sunAmount = max(dot( rayDir, sunDir ), 0.0 );
  vec3  fogColour  = mix(vec3(0.6, 0.7, 0.8), vec3(1.0,0.9, 0.7), pow(sunAmount,8.0) );

  float fogFactor = (fogMax - dist) / (fogMax - fogMin);
  fogFactor = clamp(fogFactor, 0.0, 1.0);
  fogColour = vec3(0.6,0.7, 0.8);
  return mix(vec4(fogColour, 1.0), texColour, fogFactor);

}

float calcAlpha(){
  float alpha;
  float dist = length(position.xyz);
  alpha = (fogMax - dist) / (fogMax - fogMin);
  alpha = clamp(alpha, 0.0, 1.0);
  return alpha;
}


void main(void)
{
   vec3 colour = ads();
   float alpha = calcAlpha();
   fragColour = vec4(colour * texture(grassTexture, texCoord).rgb, alpha);
   if (texture(grassTexture, texCoord).a ==0){
     discard;
   }
}
