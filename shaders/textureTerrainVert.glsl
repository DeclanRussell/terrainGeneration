#version 400

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 texCoord;

out vec3 position;
out vec3 normal;
out float height;
out vec2 TexCoords;
out float angle;

uniform sampler2D blendTexture;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;


void main(){
   if(vertexPosition.x<0.05||vertexPosition.x>0.95||vertexPosition.z<0.05||vertexPosition.z>0.95){
       vec2 heightTexCoord = texCoord/10;
       heightTexCoord-=0.005;
       heightTexCoord.y = 1.0-heightTexCoord.y;
       height = float(texture(blendTexture, heightTexCoord).r + texture(blendTexture, heightTexCoord).g + texture(blendTexture, heightTexCoord).b);
       height/=3.0;

       float interp;
       if(vertexPosition.x<0.05)
           interp = vertexPosition.x;
       else if(vertexPosition.z<0.05){
           interp = vertexPosition.z;
       }
       else if(vertexPosition.x>0.95){
           interp = vertexPosition.x - 0.95;
       }
       else if(vertexPosition.z>0.95){
           interp = vertexPosition.z - 0.95;
       }

       height = mix(height,vertexPosition.y,interp);

       position = vec3(modelViewMatrix * vec4(vertexPosition.x,height,vertexPosition.z, 1.0));
       gl_Position = modelViewProjectionMatrix * vec4(vertexPosition.x,height,vertexPosition.z, 1.0);
   }
   else{
       height = vertexPosition.y;
       position = vec3(modelViewMatrix * vec4(vertexPosition, 1.0));
       gl_Position = modelViewProjectionMatrix * vec4(vertexPosition,1.0);
   }
   angle = (1.0 - vertexNormal.y)*90.0;
   TexCoords = texCoord;
   normal = normalize(normalMatrix * vertexNormal);
}
