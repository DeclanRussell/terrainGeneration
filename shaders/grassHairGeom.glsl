#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 73) out;


in vec4 normal[];

out vec4 position;
out vec4 fragNorm;
out vec2 texCoord;

uniform float grassSize;
uniform float grassHeight;
uniform int numStrands;

in float vertHeight[];
in float vertNormalY[];
in vec2 vertTexCoord[];

uniform float maxHeight;
uniform float minHeight;
uniform float maxAngle;

uniform vec3 windDir;
uniform float windStrength;
uniform float time;

uniform mat4 modelViewProjectionMatrix;

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


void createBillboard(vec4 pos, vec4 up, vec4 right, vec4 forward){

    vec4 windTrans = vec4(windStrength * (normalize(windDir) * sin(time)),0.0);

    //create our bottom left vertex
    vec4 finalPos = pos - 0.5 * right;
    position = finalPos;
    fragNorm = normalize(forward);
    gl_Position = modelViewProjectionMatrix * finalPos;
    texCoord = vec2(0,0);
    EmitVertex();

    //top left vertex
    finalPos += up;
    position = finalPos + windTrans;
    gl_Position = modelViewProjectionMatrix * ( finalPos + windTrans );
    texCoord = vec2(0,1);
    EmitVertex();

    //bottom right vertex
    finalPos -= up;
    finalPos += right;
    position = finalPos;
    gl_Position = modelViewProjectionMatrix * finalPos;
    texCoord = vec2(1,0);
    EmitVertex();

    //top right vertex
    finalPos += up;
    position = finalPos + windTrans;
    gl_Position = modelViewProjectionMatrix * ( finalPos + windTrans );
    texCoord = vec2(1,1);
    EmitVertex();
    EndPrimitive();

    // now lets create a billboard perpendiculare to this one
    //create our bottom left vertex
    finalPos = pos - 0.5 * forward;
    position = finalPos;
    fragNorm = normalize(right);
    gl_Position = modelViewProjectionMatrix * finalPos;
    texCoord = vec2(0,0);
    EmitVertex();

    //top left vertex
    finalPos += up;
    position = finalPos + windTrans;
    gl_Position = modelViewProjectionMatrix * ( finalPos + windTrans );
    texCoord = vec2(0,1);
    EmitVertex();

    //bottom right vertex
    finalPos -= up;
    finalPos += forward;
    position = finalPos;
    gl_Position = modelViewProjectionMatrix * finalPos;
    texCoord = vec2(1,0);
    EmitVertex();

    //top right vertex
    finalPos += up;
    position = finalPos + windTrans;
    gl_Position = modelViewProjectionMatrix * ( finalPos + windTrans );
    texCoord = vec2(1,1);
    EmitVertex();
    EndPrimitive();



}

void main(void)
{
    // a little bit of optimisation, I think
    float avgPos = vertHeight[0] + vertHeight[1] + vertHeight[2];
    avgPos/=3.0;
    float avgNorm = vertNormalY[0] + vertNormalY[1] + vertNormalY[2];
    avgNorm/=3.0;
    float angle = (1.0 - avgNorm)*90.0;
    if(avgPos>minHeight && avgPos < maxHeight && angle<maxAngle){

        float A,B,C;
        for(int i=0; i<numStrands;++i){
            //gen two phsuedo random numbers
            float r1 = rand(gl_in[0].gl_Position.xy * i);
            float r2 = rand(gl_in[1].gl_Position.yz * i);

            //calculate our barycentric coeficients
            if(r1+r2>1.0){
                A = 1.0-max(r1,r2);
                B = min(r1,r2);
                C = 1.0-(A+B);
            }
            else{
                A = r1;
                B = r2;
                C = 1.0-(A+B);
            }

            //generate our new points with our barycentric coordinates

            vec4 pos = (gl_in[0].gl_Position * A) + (gl_in[1].gl_Position * B) + (gl_in[2].gl_Position * C);
            //vec4 right = vec4(cross(vec3(gl_in[0].gl_Position),vec3(gl_in[1].gl_Position)),0.0) * grassSize;
            vec4 up = (gl_in[0].gl_Position+ ((normal[0] * vertNormalY[0]) * grassHeight))*A + (gl_in[1].gl_Position+ ((normal[1] * vertNormalY[1]) * grassHeight))*B + (gl_in[2].gl_Position+ ((normal[2] * vertNormalY[2]) * grassHeight))*C;
            //vec4 scale = ((normal[0] * vertNormalY[0]) * grassSize)*A + ((normal[1] * vertNormalY[1]) * grassSize)*B + ((normal[2] * vertNormalY[2]) * grassSize)*C;
            vec4 right = vec4(normalize(cross(vec3(gl_in[0].gl_Position),vec3(gl_in[1].gl_Position))),0.0) * grassSize;
            vec4 forward = vec4(normalize(cross(vec3(up),vec3(right))),0.0) * grassSize;
            //vec4 forward = vec4(cross(vec3(right),vec3(up)),0.0) * grassSize;
            createBillboard(pos,up,right, forward);

        }
    }

}
