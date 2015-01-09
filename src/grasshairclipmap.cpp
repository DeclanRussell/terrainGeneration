#include "grasshairclipmap.h"

grassHairClipmap::grassHairClipmap()
{
    m_grassSize = 1.0;
    m_maxHeight = 1.0;
    m_grassHeight = 1.0;
    m_minHeight = 0.0;
    m_maxAngle = 90.0;
    createShader();
}
//----------------------------------------------------------------------------------------------------------------------
grassHairClipmap::grassHairClipmap(GLuint _VAO,QImage _heightMap){
    m_VAO = _VAO;
    m_grassSize = 1.0;
    m_grassHeight = 1.0;
    m_maxHeight = 1.0;
    m_minHeight = 0.0;
    m_maxAngle = 90.0;
    m_heightmapTex = new Texture(_heightMap);
    m_heightMapSet = true;
    createShader();
}
grassHairClipmap::~grassHairClipmap(){
    delete m_shaderProgram;
}

//----------------------------------------------------------------------------------------------------------------------
void grassHairClipmap::createShader(){
    //create our shader program
    m_shaderProgram = new ShaderProgram();
    Shader *vert = new Shader("shaders/grassHairClipmapVert.glsl", GL_VERTEX_SHADER);
    Shader *geom = new Shader("shaders/grassHairClipmapGeom.glsl", GL_GEOMETRY_SHADER);
    Shader *frag = new Shader("shaders/grassHairClipmapFrag.glsl", GL_FRAGMENT_SHADER);

    //attach and link our shaders to our program
    m_shaderProgram->attachShader(vert);
    m_shaderProgram->attachShader(geom);
    m_shaderProgram->attachShader(frag);
    m_shaderProgram->link();
    m_shaderProgram->use();


    //get rid of the junk
    delete vert;
    delete geom;
    delete frag;

    glEnable(GL_DEPTH_TEST); // for removal of hidden surfaces

    //set up our light

    GLuint lightPosGrassLoc = m_shaderProgram->getUniformLoc("light.position");
    GLuint lightIntGrassLoc = m_shaderProgram->getUniformLoc("light.intensity");
    GLuint kdGrassLoc = m_shaderProgram->getUniformLoc("Kd");
    GLuint kaGrassLoc = m_shaderProgram->getUniformLoc("Ka");
    GLuint ksGrassLoc = m_shaderProgram->getUniformLoc("Ks");
    GLuint shininessGrassLoc = m_shaderProgram->getUniformLoc("shininess");

    glm::vec4 lightPositon(1.0, 1.0, 1.0, 1.0);
    glm::vec3 lightIntensity(0.8, 0.8, 0.8);
    glUniform4f(lightPosGrassLoc, lightPositon.x, lightPositon.y, lightPositon.z, lightPositon.w);
    glUniform3f(lightIntGrassLoc, lightIntensity.x, lightIntensity.y, lightIntensity.z);
    glUniform3f(kdGrassLoc, 0.5, 0.5, 0.5);
    glUniform3f(kaGrassLoc, 0.5, 0.5, 0.5);
    glUniform3f(ksGrassLoc, 0.5, 0.5, 0.5);
    glUniform1f(shininessGrassLoc, 100.0);

    //set our grass size
    GLuint grassSizeLoc = m_shaderProgram->getUniformLoc("grassSize");
    GLuint grassHeightLoc = m_shaderProgram->getUniformLoc("grassHeight");
    GLuint heightMapLoc = m_shaderProgram->getUniformLoc("geoTexture");
    GLuint heightScaleLoc = m_shaderProgram->getUniformLoc("heightScale");
    m_timeLoc = m_shaderProgram->getUniformLoc("time");
    GLuint windDirLoc = m_shaderProgram->getUniformLoc("windDir");
    GLuint windStrengthLoc = m_shaderProgram->getUniformLoc("windStrength");
    GLuint scaleLoc = m_shaderProgram->getUniformLoc("scale");

    glUniform1f(heightScaleLoc,4);
    glUniform4f(scaleLoc, 2, 2, 2, 1.0);
    glUniform1f(windStrengthLoc,0.01);
    glUniform3f(windDirLoc,1.0,0.0,0.0);
    glUniform1f(m_timeLoc,clock()/CLOCKS_PER_SEC);
    glUniform1f(grassHeightLoc,m_grassHeight);
    glUniform1f(grassSizeLoc,m_grassSize);

    //set up our grass texture
    m_grassTexture = new Texture("textures/grassBlade.png");
    m_grassTexture->bind(10);
    GLuint grassBladeTexLoc = m_shaderProgram->getUniformLoc("grassTexture");
    glUniform1i(grassBladeTexLoc, 10);
    m_heightmapTex->bind(2);
    glUniform1i(heightMapLoc,2);

}

//----------------------------------------------------------------------------------------------------------------------
void grassHairClipmap::draw(glm::mat4 _modelMatrix, Camera *_cam, int _numPoints){

    m_shaderProgram->use();
    GLuint mapPosLoc = m_shaderProgram->getUniformLoc("map_position");
    glUniform4f(mapPosLoc, (-m_viewPos.x/float(2*512*32)-4), 0, (-m_viewPos.z/float(2*512*32))-4, 0);

    loadMatricesToShader(_modelMatrix,_cam);

    //update the time
    glUniform1f(m_timeLoc,clock()/1000000.0);
    m_grassTexture->bind(10);
    m_heightmapTex->bind(2);


    float sxy=2; //scale x/y
    for (int i=0; i<3; ++i){
        float ox=(int(m_viewPos.x*(1<<i))&511)/float(512*32);
        float oz=(int(m_viewPos.z*(1<<i))&511)/float(512*32);


        glm::vec3 scale(sxy, sxy, sxy);
        GLuint scaleLoc = m_shaderProgram->getUniformLoc("scale");
        glUniform4f(scaleLoc, scale.x, scale.y, scale.z, 1.0);


        for (int k=-2; k<2; ++k) {
            for (int j=-2; j<2; ++j){

                if (i!=2) if (k==-1 || k==0) if (j==-1||j==0) continue;

                glm::vec3 offset(ox+float(j), 0, oz+float(k));
                if (k>=0) offset.z-=1.0/float(32); //adjust offset for proper overlapping
                if (j>=0) offset.x-=1.0/float(32); // adjust offset for proper overlapping

                // render
                GLuint offsetLoc = m_shaderProgram->getUniformLoc("offset");
                glUniform4f(offsetLoc, offset.x, 0.0, offset.z, 0.0);

                glBindVertexArray(m_VAO);
                glPointSize(20.0);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, _numPoints);
                glBindVertexArray(0);
            }
        }
        sxy*=0.5;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void grassHairClipmap::loadMatricesToShader(glm::mat4 _modelMatrix, Camera *_cam){
    // Calculate projection matrix
    int scale = 1;
    _modelMatrix = glm::scale(_modelMatrix, glm::vec3(scale,scale,scale));
    glm::mat4 projectionMatrix = _cam->getProjectionMatrix();

    glm::mat4 modelViewMatrix = _cam->getViewMatrix() * _modelMatrix;

    glm::mat3 normalMatrix = glm::mat3(modelViewMatrix);
    normalMatrix = glm::inverse(normalMatrix);
    normalMatrix = glm::transpose(normalMatrix);

    glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    m_shaderProgram->use();
    // Find uniform locations
    GLuint modelViewLoc = m_shaderProgram->getUniformLoc("modelViewMatrix");
    GLuint projLoc = m_shaderProgram->getUniformLoc("projectionMatrix");
    GLuint normalLoc = m_shaderProgram->getUniformLoc("normalMatrix");
    GLuint modelViewProjectionLoc = m_shaderProgram->getUniformLoc("modelViewProjectionMatrix");
    GLuint grassSizeLoc = m_shaderProgram->getUniformLoc("grassSize");
    GLuint grassHeightLoc = m_shaderProgram->getUniformLoc("grassHeight");
    GLuint maxHeightLoc = m_shaderProgram->getUniformLoc("maxHeight");
    GLuint minHeightLoc = m_shaderProgram->getUniformLoc("minHeight");
    GLuint maxAngleLoc = m_shaderProgram->getUniformLoc("maxAngle");
    GLuint numStrandsLoc = m_shaderProgram->getUniformLoc("numStrands");

    glUniform1f(grassHeightLoc,m_grassHeight);
    glUniform1i(numStrandsLoc,m_numStrandsPerFace);
    glUniform1f(maxHeightLoc,m_maxHeight);
    glUniform1f(minHeightLoc,m_minHeight);
    glUniform1f(maxAngleLoc,m_maxAngle);
    glUniform1f(grassSizeLoc,m_grassSize);


    //glUniformMatrix4fv(m_modelLoc, 1, false, glm::value_ptr(m_modelMatrix));
    glUniformMatrix4fv(modelViewLoc, 1, false, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(projLoc, 1, false, glm::value_ptr(projectionMatrix));
    glUniformMatrix3fv(normalLoc, 1, false, glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(modelViewProjectionLoc, 1, false, glm::value_ptr(modelViewProjectionMatrix));
}

//----------------------------------------------------------------------------------------------------------------------
