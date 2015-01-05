#version 400

in vec3 position;
in vec3 normal;
in vec2 TexCoords;
in float angle;

struct lightInfo{
   vec4 position;
   vec3 intensity;
};

uniform lightInfo light;

uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform float shininess;

//our texture samplers
uniform sampler2D mudTexture;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;
//our height bounderies for shading different materials
uniform float mudHeight;
uniform float grassHeight;
uniform float rockHeight;
//set our bounds for mixing between 2 materials
uniform float mixThreshhold;
//the height of our vertex unchanged by any mvp matricies
in float height;

//our output colour
out vec4 fragColour;

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
    vec4 mudColour = texture(mudTexture,TexCoords);
    vec4 grassColour = texture(grassTexture,TexCoords);
    vec4 rockColour = texture(rockTexture,TexCoords);
    vec4 snowColour = texture(snowTexture,TexCoords);

    vec4 finalColour;

    if(height<=mudHeight){
        finalColour = mudColour;
        float x = mudHeight-height;
        if(x<=mixThreshhold){
            finalColour = mix(grassColour,finalColour,x/mixThreshhold);
        }
    }
    else if(height>mudHeight && height<=grassHeight){
        finalColour = grassColour;
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
        finalColour = snowColour;
    }
//    if(angle>30){
//        finalColour = mix(finalColour,rockColour,((angle-30.0)/60));
//    }


    return finalColour;
}

void  main(){
   vec4 colour = calcColour();
   colour = colour*vec4(ads(),1.0);
   fragColour = colour;// * texture(tex, TexCoords);
}
