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


void main(void)
{
   vec4 colour = vec4(ads(),1.0);
   fragColour = colour * texture(grassTexture, texCoord);
   if (texture(grassTexture, texCoord).a ==0){
     discard;
   }
}
