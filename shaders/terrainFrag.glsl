#version 400

in vec3 position;
in vec3 normal;
//in vec2 TexCoords;

struct lightInfo{
   vec4 position;
   vec3 intensity;
};

uniform lightInfo light;

uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform float shininess;
in float height;
out vec4 fragColour;
in float angle;


uniform vec4 mudColour;
uniform vec4 grassColour;
uniform vec4 rockColour;
uniform vec4 snowColour;

uniform float mudHeight;
uniform float grassHeight;
uniform float rockHeight;
uniform float snowHeight;


vec3 ads(){
   vec3 n = normalize(normal);
   vec3 s = normalize(vec3(light.position) - position);
   vec3 v = normalize(vec3(-position));
   vec3 r = reflect(-s, n);
   vec3 h = normalize(v + s);
   return light.intensity * (Ka + Kd * max(dot(s,n),0.0)+ Ks * pow(max(dot(h, n), 0.0), shininess));
}
// a function inspired by "A Rule-based Approach to 3D Terrain Generation via Texture Splatting"
// written by Jonathan Ferraris & Christos Gatzidis
vec4 calcColour(){
    vec4 finalColour;
    float mixThreshhold = 0.05;
        if(height<=mudHeight){
//            if(angle>30.0){
//                finalColour = mix(mudColour,rockColour,((angle-30.0)/60));
//            }
//            else{
                finalColour = mudColour;
//            }
            float x = mudHeight-height;
            if(x<=mixThreshhold){
                finalColour = mix(grassColour,finalColour,x/mixThreshhold);
            }
        }
        else if(height>mudHeight && height<=grassHeight){
//            if(angle>30.0){
//                finalColour = mix(grassColour,rockColour,((angle-30.0)/60));
//            }
//            else{
                finalColour = grassColour;
//            }
            float x = grassHeight-height;
            if(x<=mixThreshhold){
                finalColour = mix(rockColour,finalColour,x/mixThreshhold);
            }
        }
        else if(height>grassHeight && height<=rockHeight){
            float x = rockHeight-height;
            if(x<=mixThreshhold){
                finalColour = mix(snowColour,rockColour,x/mixThreshhold);
            }
            else{
                finalColour = rockColour;
            }
        }
        else if(height>rockHeight){
//            if(angle>30.0){
//                finalColour = mix(snowColour,rockColour,((angle-30.0)/60));
//            }
//            else{
                finalColour = snowColour;
//            }
        }
    return finalColour;
}

void  main(){
   vec4 colour = calcColour();
   colour = colour*vec4(ads(),1.0);
   fragColour = colour;// * texture(tex, TexCoords);
}
