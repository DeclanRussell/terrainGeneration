#include "Grass.h"


//----------------------------------------------------------------------------------------------------------------------
Grass::Grass(int _numGrass, terrainGen::ElmT **_matStack, int _sizeX, int _sizeY){
    m_sampleType = G_MATSTACK;
    m_numGrass = _numGrass;
    m_matStackData = _matStack;
    m_matStackSizeX = _sizeX;
    m_matStackSizeY = _sizeY;
    m_maxSpawnHeight = 1.0;
    m_minSpawnHeight = 0.0;
    createShader();
}

//----------------------------------------------------------------------------------------------------------------------
Grass::Grass(int _numGrass, QImage _heightmap){
    m_sampleType = G_HEGIHTMAP;
    m_numGrass = _numGrass;
    m_heightmap = _heightmap;
    m_maxSpawnHeight = 1.0;
    m_minSpawnHeight = 0.0;
    createShader();
}
//----------------------------------------------------------------------------------------------------------------------
Grass::~Grass(){
}
//----------------------------------------------------------------------------------------------------------------------
void Grass::    createShader(){
    m_grassShader  = new ShaderProgram();
    m_grassVert = new Shader("shaders/grassVert.glsl", GL_VERTEX_SHADER);
    m_grassFrag = new Shader("shaders/grassFrag.glsl", GL_FRAGMENT_SHADER);
    m_grassShader->attachShader(m_grassVert);
    m_grassShader->attachShader(m_grassFrag);
    m_grassShader->bindFragDataLocation(0, "fragColour");
    m_grassShader->link();
    m_grassShader->use();

    delete m_grassVert;
    delete m_grassFrag;

    m_grassBladeTex = new Texture("textures/grassBlade.png");
    m_grassBladeTex->bind(8);


    GLuint lightPosGrassLoc = m_grassShader->getUniformLoc("light.position");
    GLuint lightIntGrassLoc = m_grassShader->getUniformLoc("light.intensity");
    GLuint kdGrassLoc = m_grassShader->getUniformLoc("Kd");
    GLuint kaGrassLoc = m_grassShader->getUniformLoc("Ka");
    GLuint ksGrassLoc = m_grassShader->getUniformLoc("Ks");
    GLuint shininessGrassLoc = m_grassShader->getUniformLoc("shininess");
    GLuint grassBladeTexLoc = m_grassShader->getUniformLoc("tex");

    glm::vec4 lightPositon(1.0, 1.0, 1.0, 1.0);
    glm::vec3 lightIntensity(0.8, 0.8, 0.8);
    glUniform4f(lightPosGrassLoc, lightPositon.x, lightPositon.y, lightPositon.z, lightPositon.w);
    glUniform3f(lightIntGrassLoc, lightIntensity.x, lightIntensity.y, lightIntensity.z);
    glUniform3f(kdGrassLoc, 0.5, 0.5, 0.5);
    glUniform3f(kaGrassLoc, 0.5, 0.5, 0.5);
    glUniform3f(ksGrassLoc, 0.5, 0.5, 0.5);
    glUniform1f(shininessGrassLoc, 100.0);
    glUniform1i(grassBladeTexLoc, 8);
}
//----------------------------------------------------------------------------------------------------------------------
void Grass::createGrass(){
    m_model = new Model("models/grass.obj");
    glBindVertexArray(m_model->getVAO());

    GLfloat offset[m_numGrass*3];
    srand(time(NULL));

    // COULD BE SPED UP USING TRANSFORM FEEDBACK !!  
    float x,y,z,normalY;
    std::vector<glm::vec3> normals;
    for (int i=0; i<m_numGrass*3; i+=3){
        x = (0.0) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((1.0)-(0.0))));
        z = (0.0) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((1.0)-(0.0))));
        if(m_sampleType == G_HEGIHTMAP){
            y = getHeightFromTexture((int)(x * (m_heightmap.width()-1)), (int)(z * (m_heightmap.height()-1)));
            normalY = getNormalFromHeightMap((int)(x * (m_heightmap.width()-1)),(int)(z * (m_heightmap.height()-1))).y;
        }
        else{
            y = getHeightFromMatStack((int)(x * m_matStackSizeX-1),(int)(z * m_matStackSizeY-1));
            normalY = getNormalFromMatStack((int)(x * m_matStackSizeX-1),(int)(z * m_matStackSizeY-1)).y;
        }

        while(normalY<0.8 || (y>m_maxSpawnHeight || y<m_minSpawnHeight)){
            x = (0.0) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((1.0)-(0.0))));
            z = (0.0) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((1.0)-(0.0))));
            if(m_sampleType == G_HEGIHTMAP){
                y = getHeightFromTexture((int)(x * (m_heightmap.width()-1)), (int)(z * (m_heightmap.height()-1)));
                normalY = getNormalFromHeightMap((int)(x * (m_heightmap.width()-1)),(int)(z * (m_heightmap.height()-1))).y;
            }
            else{
                y = getHeightFromMatStack((int)(x * m_matStackSizeX-1),(int)(z * m_matStackSizeY-1));
                normalY = getNormalFromMatStack((int)(x * m_matStackSizeX-1),(int)(z * m_matStackSizeY-1)).y;
            }
        }

        offset[i] = x;
        offset[i+1] = y;
        offset[i+2] = z;
        m_positions.push_back(glm::vec2(x, z));
        normals.push_back(getNormalFromHeightMap(m_positions[i].x* (m_heightmap.width()-1), m_positions[i].y * (m_heightmap.height()-1)));
    }

    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(offset), &offset[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER,0);

    GLuint terrainNormal;
    glGenBuffers(1, &terrainNormal);
    glBindBuffer(GL_ARRAY_BUFFER, terrainNormal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), &normals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glVertexAttribDivisor(4, 1);
    glBindVertexArray(0);
    glDeleteBuffers(1, &positionBuffer);
}
//----------------------------------------------------------------------------------------------------------------------
float Grass::getHeightFromMatStack(int _x, int _y){
    bool surfaceFound = false;
    float heightSum = 0.0;
    for(int i=(m_matStackData[_x][_y].properties.size()-1); i>=0;i--){
        if(m_matStackData[_x][_y].properties[i].type==terrainGen::BEDROCK  ||  m_matStackData[_x][_y].properties[i].type==terrainGen::SAND)
        surfaceFound = true;
        if(surfaceFound){
            heightSum+=m_matStackData[_x][_y].properties[i].height;
        }
    }
    return heightSum;
}
//----------------------------------------------------------------------------------------------------------------------
glm::vec3 Grass::getNormalFromMatStack(int _xPos, int _yPos){
    glm::vec3 normals;
    int fX = _xPos;
    int fZ = _yPos;
    if(  (fX>0&&fZ>0)  &&  (fX<(m_matStackSizeX-1)&&(fZ<(m_matStackSizeY-1)))    ){
        glm::vec3 vMid,vUp,vL,vR,vDwn,v1,v2,n1,n2,n3,n4,finalNormal;
        float height;
        //center point
        height = getHeightFromMatStack(fX,fZ);
        vMid = glm::vec3(((float)fX/(float)m_matStackSizeX),height,((float)fZ/(float)m_matStackSizeY));
        //point above
        height = getHeightFromMatStack(fX,fZ+1);
        vUp = glm::vec3(((float)fX/(float)m_matStackSizeX),height,((float)fZ+1/(float)m_matStackSizeY));
        //point to the left
        height = getHeightFromMatStack(fX-1,fZ);
        vL = glm::vec3(((float)fX-1/(float)m_matStackSizeX),height,((float)fZ/(float)m_matStackSizeY));
        //point to the right
        height = getHeightFromMatStack(fX+1,fZ);
        vR = glm::vec3(((float)fX+1/(float)m_matStackSizeX),height,((float)fZ/(float)m_matStackSizeY));
        //point below
        height = getHeightFromMatStack(fX,fZ-1);
        vDwn = glm::vec3(((float)fX/(float)m_matStackSizeX),height,((float)fZ-1/(float)m_matStackSizeY));

        //now lets calulate some face normals
        //top left face
        v1 = vUp-vMid;
        v2 = vL-vMid;
        n1 = glm::cross(v1,v2);
        //top right face
        v1 = vR-vMid;
        v2 = vUp-vMid;
        n2 = glm::cross(v1,v2);
        //bottom right
        v1 = vDwn-vMid;
        v2 = vR-vMid;
        n3 = glm::cross(v1,v2);
        //bottom left
        v1 = vL-vMid;
        v2 = vDwn-vMid;
        n4 = glm::cross(v1,v2);


        //now we have our face normals let calculate our vertex normal
        finalNormal = glm::normalize(n1+n2+n3+n4);
        //cos Im a scrub I work my normals out upside down so we need to flip them
        finalNormal*=-1;
        normals = glm::vec3(finalNormal.x, finalNormal.y, finalNormal.z);
    }
    else{
        normals = glm::vec3(0.0, 0.5, 0.0);

    }
    return normals;
}

//----------------------------------------------------------------------------------------------------------------------
float Grass::getHeightFromTexture(int _width, int _depth){
    QColor _pixel = m_heightmap.pixel(_width, _depth);
    return ((_pixel.redF()+_pixel.blueF()+_pixel.greenF())/3.0);
}
//----------------------------------------------------------------------------------------------------------------------
glm::vec3 Grass::getNormalFromHeightMap(int _xPos , int _yPos){
    glm::vec3 normals;
    int fX = _xPos;
    int fZ = _yPos;
    if(  (fX>0&&fZ>0)  &&  (fX<(m_heightmap.width()-1)&&(fZ<(m_heightmap.height()-1)))    ){
        glm::vec3 vMid,vUp,vL,vR,vDwn,v1,v2,n1,n2,n3,n4,finalNormal;
        QColor pixelData;
        //center point
        pixelData = m_heightmap.pixel(fX,fZ);
        vMid = glm::vec3((float)fX,(float)pixelData.red(),(float)fZ);
        //point above
        pixelData = m_heightmap.pixel(fX,fZ+1);
        vUp = glm::vec3((float)fX,(float)pixelData.red(),(float)fZ+1);
        //point to the left
        pixelData = m_heightmap.pixel(fX-1,fZ);
        vL = glm::vec3((float)fX-1,(float)pixelData.red(),(float)fZ);
        //point to the right
        pixelData = m_heightmap.pixel(fX+1,fZ);
        vR = glm::vec3((float)fX+1,(float)pixelData.red(),(float)fZ);
        //point below
        pixelData = m_heightmap.pixel(fX,fZ-1);
        vDwn = glm::vec3((float)fX,(float)pixelData.red(),(float)fZ-1);

        //now lets calulate some face normals
        //top left face
        v1 = vUp-vMid;
        v2 = vL-vMid;
        n1 = glm::cross(v1,v2);
        //top right face
        v1 = vR-vMid;
        v2 = vUp-vMid;
        n2 = glm::cross(v1,v2);
        //bottom right
        v1 = vDwn-vMid;
        v2 = vR-vMid;
        n3 = glm::cross(v1,v2);
        //bottom left
        v1 = vL-vMid;
        v2 = vDwn-vMid;
        n4 = glm::cross(v1,v2);


        //now we have our face normals let calculate our vertex normal
        finalNormal = glm::normalize(n1+n2+n3+n4);
        //becuase Im a scrub I worked them out upside down
        finalNormal*=-1;
        normals = glm::vec3(finalNormal.x, finalNormal.y, finalNormal.z);
    }
    else{
        normals = glm::vec3(0.0, 1.0, 0.0);

    }
    return normals;
}
//----------------------------------------------------------------------------------------------------------------------
void Grass::render(){
    glBindVertexArray(m_model->getVAO());
    glDrawArraysInstanced(GL_TRIANGLES, 0, m_model->getNumVerts(), m_numGrass);
    glBindVertexArray(0);

}
//----------------------------------------------------------------------------------------------------------------------
void Grass::loadMatricesToShader(glm::mat4 _modelMatrix,Camera* _cam){
    // Calculate projection matrix
    int scale = 1;
    _modelMatrix = glm::scale(_modelMatrix, glm::vec3(scale,scale,scale));
    glm::mat4 projectionMatrix = _cam->getProjectionMatrix();

    glm::mat4 modelViewMatrix = _cam->getViewMatrix() * _modelMatrix;

    glm::mat3 normalMatrix = glm::mat3(modelViewMatrix);
    normalMatrix = glm::inverse(normalMatrix);
    normalMatrix = glm::transpose(normalMatrix);

    glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    m_grassShader->use();
    // Find uniform locations
    GLuint modelLoc = m_grassShader->getUniformLoc("modelMatrix");
    GLuint viewLoc = m_grassShader->getUniformLoc("viewMatrix");
    GLuint modelViewLoc = m_grassShader->getUniformLoc("modelViewMatrix");
    GLuint projLoc = m_grassShader->getUniformLoc("projectionMatrix");
    GLuint normalLoc = m_grassShader->getUniformLoc("normalMatrix");
    GLuint modelViewProjectionLoc = m_grassShader->getUniformLoc("modelViewProjectionMatrix");

    glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(_modelMatrix));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(_cam->getViewMatrix()));
    glUniformMatrix4fv(modelViewLoc, 1, false, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(projLoc, 1, false, glm::value_ptr(projectionMatrix));
    glUniformMatrix3fv(normalLoc, 1, false, glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(modelViewProjectionLoc, 1, false, glm::value_ptr(modelViewProjectionMatrix));
}
//----------------------------------------------------------------------------------------------------------------------
void Grass::updateGrass( QString _pathToHeightMap){
    m_heightmap = QImage(_pathToHeightMap);
    m_sampleType = G_HEGIHTMAP;
    m_model->deleteVAO();
    m_positions.clear();
    createGrass();
}
//----------------------------------------------------------------------------------------------------------------------
