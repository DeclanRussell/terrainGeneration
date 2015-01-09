#include "include/marchingcubes.h"
#include <fstream>
#include <QColor>
#include <glm/glm.hpp>

marchingCubes::marchingCubes() : m_wireframe(false)
{
    iDataSetSize = 128;
    fStepSize = 1.0/(float)iDataSetSize;
    fTargetValue = 0;//48.0;
    fTime = 0.0;
    m_samplePercent.first = 1.0;
    m_samplePercent.second = 1.0;
    m_samplePos.first = 0.0;
    m_samplePos.second = 0.0;
    m_sampleMode = MCTEXTURE;
    m_position.clear();
    m_normals.clear();
    m_texCoords.clear();
    m_allocatedData = false;
    m_blendTexSet = false;
}
marchingCubes::~marchingCubes(){
    delete m_shaderProgram;
    glDeleteBuffers(2,m_vboHandle);
    glDeleteVertexArrays(1,&m_mCubesVAO);
}

//----------------------------------------------------------------------------------------------------------------------
void marchingCubes::exportGeometryObj(std::string _name){
    if (m_position.size()>0){


        std::ofstream myFile;
        std::string objName = _name + ".obj";
        myFile.open(objName.data());



        for(unsigned int i=0; i<m_position.size();i++){
            myFile<<"v "<<m_position[i].x<<" "<<m_position[i].y<<" "<<m_position[i].z<<""<<std::endl;
        }
        for(unsigned int i=0; i<m_normals.size();i++){
            myFile<<"vn "<<m_normals[i].x<<" "<<m_normals[i].y<<" "<<m_normals[i].z<<""<<std::endl;
        }
        for(unsigned int i=0; i<m_texCoords.size(); i++){
            myFile<<"vt "<<m_texCoords[i].x<<" "<<m_texCoords[i].y<<" "<<std::endl;
        }
        for(unsigned int i=0; i<m_position.size()/3;i++){
            myFile<<"f "<<i*3+1<<" "<<(i*3)+2<<" "<<(i*3)+3<<""<<std::endl;
        }
        myFile.close();
    }
}
//----------------------------------------------------------------------------------------------------------------------
void marchingCubes::createShader(){
    // Create a shader program
    m_shaderProgram = new ShaderProgram();
    m_vertexShader = new Shader("shaders/textureTerrainVert.glsl", GL_VERTEX_SHADER);
    m_fragmentShader = new Shader("shaders/textureTerrainFrag.glsl", GL_FRAGMENT_SHADER);

    m_shaderProgram->attachShader(m_vertexShader);
    m_shaderProgram->attachShader(m_fragmentShader);
    m_shaderProgram->bindFragDataLocation(0, "fragColour");
    m_shaderProgram->link();
    m_shaderProgram->use();

    delete m_vertexShader;
    delete m_fragmentShader;

    // Find uniform locations
    m_modelViewLoc = m_shaderProgram->getUniformLoc("modelViewMatrix");
    m_projLoc = m_shaderProgram->getUniformLoc("projectionMatrix");
    m_normalLoc = m_shaderProgram->getUniformLoc("normalMatrix");
    m_modelViewProjectionLoc = m_shaderProgram->getUniformLoc("modelViewProjectionMatrix");

    // set our light uniforms
    GLuint lightPosLoc = m_shaderProgram->getUniformLoc("light.position");
    GLuint lightIntLoc = m_shaderProgram->getUniformLoc("light.intensity");
    GLuint kdLoc = m_shaderProgram->getUniformLoc("Kd");
    GLuint kaLoc = m_shaderProgram->getUniformLoc("Ka");
    GLuint ksLoc = m_shaderProgram->getUniformLoc("Ks");
    GLuint shininessLoc = m_shaderProgram->getUniformLoc("shininess");

    glUniform4f(lightPosLoc, 1.0, 1.0, 1.0, 1.0);
    glUniform3f(lightIntLoc, 0.8, 0.8, 0.8);
    glUniform3f(kdLoc, 0.5, 0.5, 0.5);
    glUniform3f(kaLoc, 0.5, 0.5, 0.5);
    glUniform3f(ksLoc, 0.5, 0.5, 0.5);
    glUniform1f(shininessLoc, 1.0);

    // create our textures
    m_mudTex = new Texture("textures/mudTexture");
    m_mudTex->bind(0);

    m_grassTex = new Texture("textures/grassTexture");
    m_grassTex->bind(1);

    m_rockTex = new Texture("textures/rockTexture");
    m_rockTex->bind(2);

    m_snowTex = new Texture("textures/snowTexture");
    m_snowTex->bind(3);

    m_blendTexture->bind(4);

    // Set our mesh colouring information based on texture splatting
    GLuint mudHeight = m_shaderProgram->getUniformLoc("mudHeight");
    GLuint grassHeight = m_shaderProgram->getUniformLoc("grassHeight");
    GLuint rockHeight = m_shaderProgram->getUniformLoc("rockHeight");

    //set our height bands
    glUniform1f(mudHeight,0.4);
    glUniform1f(grassHeight,0.5);
    glUniform1f(rockHeight,0.7);

    // set our texture locations in our shader
    GLuint mudTexLoc = m_shaderProgram->getUniformLoc("mudTexture");
    GLuint grassTexLoc = m_shaderProgram->getUniformLoc("grassTexture");
    GLuint rockTexLoc = m_shaderProgram->getUniformLoc("rockTexture");
    GLuint snowTexLoc = m_shaderProgram->getUniformLoc("snowTexture");
    GLuint blendTexLoc = m_shaderProgram->getUniformLoc("blendTexture");

    glUniform1i(mudTexLoc, 0);
    glUniform1i(grassTexLoc, 1);
    glUniform1i(rockTexLoc, 2);
    glUniform1i(snowTexLoc, 3);
    glUniform1i(blendTexLoc,4);

    //set our mix threshhold to set the bounds that we want to blend between 2 materials
    GLuint mixThreshhold = m_shaderProgram->getUniformLoc("mixThreshhold");
    glUniform1f(mixThreshhold, 0.05);
}
//----------------------------------------------------------------------------------------------------------------------
void marchingCubes::allocateGeomtryData(){
    // create our vertex array object
    glGenVertexArrays(1,&m_mCubesVAO);
    glBindVertexArray(m_mCubesVAO);
    // genertate our 2 VBO's for normal and vertex data
    glGenBuffers(3,m_vboHandle);

    // create our vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER,m_vboHandle[0]);
    glBufferData(GL_ARRAY_BUFFER, m_position.size()*3*sizeof(GLfloat),&m_position[0].x,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),(GLvoid*)(0*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(0);

    std::cout<<"the number of points is "<<m_position.size()<<std::endl;
    std::cout<<"the number of normals is "<<m_normals.size()<<std::endl;

    // create our normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vboHandle[1]);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size()*3*sizeof(GLfloat),&m_normals[0].x,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_TRUE,sizeof(glm::vec3),(GLvoid*)(0*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // create our texture coords buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vboHandle[2]);
    glBufferData(GL_ARRAY_BUFFER, m_texCoords.size()*2*sizeof(GLfloat),&m_texCoords[0].x,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_TRUE,sizeof(glm::vec2),(GLvoid*)(0*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);

    m_allocatedData = true;
}

//----------------------------------------------------------------------------------------------------------------------
void marchingCubes::updateGeometry(){
    glBindVertexArray(m_mCubesVAO);

    // create our vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER,m_vboHandle[0]);
    glBufferData(GL_ARRAY_BUFFER, m_position.size()*3*sizeof(GLfloat),&m_position[0].x,GL_DYNAMIC_DRAW);

    // create our normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vboHandle[1]);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size()*3*sizeof(GLfloat),&m_normals[0].x,GL_DYNAMIC_DRAW);

    // create our texture coords buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vboHandle[2]);
    glBufferData(GL_ARRAY_BUFFER, m_texCoords.size()*2*sizeof(GLfloat),&m_texCoords[0].x,GL_DYNAMIC_DRAW);
}

//----------------------------------------------------------------------------------------------------------------------
void marchingCubes::draw(glm::mat4 _modelMatrix, Camera *_cam){
    loadMatricesToShader(_modelMatrix,_cam);

    m_mudTex->bind(0);
    m_grassTex->bind(1);
    m_rockTex->bind(2);
    m_snowTex->bind(3);
    m_blendTexture->bind(4);

//    if (m_wireframe){
//        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
//    }
//    else{
//        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
//    }
    glBindVertexArray(m_mCubesVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_position.size());
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------------------------
void marchingCubes::loadMatricesToShader(glm::mat4 _modelMatrix, Camera *_cam){
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
    m_modelViewLoc = m_shaderProgram->getUniformLoc("modelViewMatrix");
    m_projLoc = m_shaderProgram->getUniformLoc("projectionMatrix");
    m_normalLoc = m_shaderProgram->getUniformLoc("normalMatrix");
    m_modelViewProjectionLoc = m_shaderProgram->getUniformLoc("modelViewProjectionMatrix");

    //glUniformMatrix4fv(m_modelLoc, 1, false, glm::value_ptr(m_modelMatrix));
    glUniformMatrix4fv(m_modelViewLoc, 1, false, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(m_projLoc, 1, false, glm::value_ptr(projectionMatrix));
    glUniformMatrix3fv(m_normalLoc, 1, false, glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(m_modelViewProjectionLoc, 1, false, glm::value_ptr(modelViewProjectionMatrix));
}

//----------------------------------------------------------------------------------------------------------------------
//These tables are used so that everything can be done in little loops that you can look at all at once
// rather than in pages and pages of unrolled code.

//a2fVertexOffset lists the positions, relative to vertex0, of each of the 8 vertices of a cube
static const GLfloat a2fVertexOffset[8][3] =
{
    {0.0, 0.0, 0.0},{1.0, 0.0, 0.0},{1.0, 1.0, 0.0},{0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0},{1.0, 0.0, 1.0},{1.0, 1.0, 1.0},{0.0, 1.0, 1.0}
};
//----------------------------------------------------------------------------------------------------------------------
//a2iEdgeConnection lists the index of the endpoint vertices for each of the 12 edges of the cube
static const GLint a2iEdgeConnection[12][2] =
{
        {0,1}, {1,2}, {2,3}, {3,0},
        {4,5}, {5,6}, {6,7}, {7,4},
        {0,4}, {1,5}, {2,6}, {3,7}
};
//----------------------------------------------------------------------------------------------------------------------
//a2fEdgeDirection lists the direction vector (vertex1-vertex0) for each edge in the cube
static const GLfloat a2fEdgeDirection[12][3] =
{
        {1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{-1.0, 0.0, 0.0},{0.0, -1.0, 0.0},
        {1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{-1.0, 0.0, 0.0},{0.0, -1.0, 0.0},
        {0.0, 0.0, 1.0},{0.0, 0.0, 1.0},{ 0.0, 0.0, 1.0},{0.0,  0.0, 1.0}
};
//----------------------------------------------------------------------------------------------------------------------
//a2iTetrahedronEdgeConnection lists the index of the endpoint vertices for each of the 6 edges of the tetrahedron
static const GLint a2iTetrahedronEdgeConnection[6][2] =
{
        {0,1},  {1,2},  {2,0},  {0,3},  {1,3},  {2,3}
};
//----------------------------------------------------------------------------------------------------------------------
//a2iTetrahedronEdgeConnection lists the index of verticies from a cube
// that made up each of the six tetrahedrons within the cube
static const GLint a2iTetrahedronsInACube[6][4] =
{
        {0,5,1,6},
        {0,1,2,6},
        {0,2,3,6},
        {0,3,7,6},
        {0,7,4,6},
        {0,4,5,6},
};
//----------------------------------------------------------------------------------------------------------------------



// For any edge, if one vertex is inside of the surface and the other is outside of the surface
//  then the edge intersects the surface
// For each of the 4 vertices of the tetrahedron can be two possible states : either inside or outside of the surface
// For any tetrahedron the are 2^4=16 possible sets of vertex states
// This table lists the edges intersected by the surface for all 16 possible vertex states
// There are 6 edges.  For each entry in the table, if edge #n is intersected, then bit #n is set to 1

GLint aiTetrahedronEdgeFlags[16]=
{
        0x00, 0x0d, 0x13, 0x1e, 0x26, 0x2b, 0x35, 0x38, 0x38, 0x35, 0x2b, 0x26, 0x1e, 0x13, 0x0d, 0x00,
};
//----------------------------------------------------------------------------------------------------------------------

// For each of the possible vertex states listed in aiTetrahedronEdgeFlags there is a specific triangulation
// of the edge intersection points.  a2iTetrahedronTriangles lists all of them in the form of
// 0-2 edge triples with the list terminated by the invalid value -1.
//
// I generated this table by hand

GLint a2iTetrahedronTriangles[16][7] =
{
        {-1, -1, -1, -1, -1, -1, -1},
        { 0,  3,  2, -1, -1, -1, -1},
        { 0,  1,  4, -1, -1, -1, -1},
        { 1,  4,  2,  2,  4,  3, -1},

        { 1,  2,  5, -1, -1, -1, -1},
        { 0,  3,  5,  0,  5,  1, -1},
        { 0,  2,  5,  0,  5,  4, -1},
        { 5,  4,  3, -1, -1, -1, -1},

        { 3,  4,  5, -1, -1, -1, -1},
        { 4,  5,  0,  5,  2,  0, -1},
        { 1,  5,  0,  5,  3,  0, -1},
        { 5,  2,  1, -1, -1, -1, -1},

        { 3,  4,  2,  2,  4,  1, -1},
        { 4,  1,  0, -1, -1, -1, -1},
        { 2,  3,  0, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1},
};
//----------------------------------------------------------------------------------------------------------------------
// For any edge, if one vertex is inside of the surface and the other is outside of the surface
//  then the edge intersects the surface
// For each of the 8 vertices of the cube can be two possible states : either inside or outside of the surface
// For any cube the are 2^8=256 possible sets of vertex states
// This table lists the edges intersected by the surface for all 256 possible vertex states
// There are 12 edges.  For each entry in the table, if edge #n is intersected, then bit #n is set to 1

GLint aiCubeEdgeFlags[256]=
{
        0x000, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c, 0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
        0x190, 0x099, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c, 0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
        0x230, 0x339, 0x033, 0x13a, 0x636, 0x73f, 0x435, 0x53c, 0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
        0x3a0, 0x2a9, 0x1a3, 0x0aa, 0x7a6, 0x6af, 0x5a5, 0x4ac, 0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
        0x460, 0x569, 0x663, 0x76a, 0x066, 0x16f, 0x265, 0x36c, 0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
        0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0x0ff, 0x3f5, 0x2fc, 0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
        0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x055, 0x15c, 0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
        0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0x0cc, 0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
        0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc, 0x0cc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
        0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c, 0x15c, 0x055, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
        0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc, 0x2fc, 0x3f5, 0x0ff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
        0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c, 0x36c, 0x265, 0x16f, 0x066, 0x76a, 0x663, 0x569, 0x460,
        0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac, 0x4ac, 0x5a5, 0x6af, 0x7a6, 0x0aa, 0x1a3, 0x2a9, 0x3a0,
        0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c, 0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x033, 0x339, 0x230,
        0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c, 0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x099, 0x190,
        0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c, 0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x000
};
//----------------------------------------------------------------------------------------------------------------------
//  For each of the possible vertex states listed in aiCubeEdgeFlags there is a specific triangulation
//  of the edge intersection points.  a2iTriangleConnectionTable lists all of them in the form of
//  0-5 edge triples with the list terminated by the invalid value -1.
//  For example: a2iTriangleConnectionTable[3] list the 2 triangles formed when corner[0]
//  and corner[1] are inside of the surface, but the rest of the cube is not.
//
//  I found this table in an example program someone wrote long ago.  It was probably generated by hand

GLint a2iTriangleConnectionTable[256][16] =
{
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
        {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
        {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
        {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
        {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
        {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
        {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
        {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
        {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
        {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
        {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
        {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
        {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
        {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
        {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
        {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
        {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
        {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
        {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
        {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
        {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
        {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
        {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
        {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
        {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
        {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
        {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
        {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
        {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
        {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
        {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
        {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
        {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
        {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
        {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
        {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
        {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
        {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
        {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
        {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
        {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
        {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
        {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
        {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
        {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
        {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
        {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
        {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
        {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
        {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
        {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
        {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
        {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
        {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
        {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
        {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
        {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
        {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
        {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
        {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
        {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
        {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
        {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
        {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
        {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
        {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
        {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
        {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
        {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
        {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
        {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
        {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
        {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
        {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
        {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
        {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
        {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
        {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
        {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
        {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
        {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
        {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
        {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
        {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
        {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
        {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
        {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
        {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
        {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
        {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
        {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
        {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
        {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
        {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
        {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
        {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
        {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
        {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
        {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
        {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
        {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
        {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
        {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
        {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
        {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
        {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
        {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
        {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
        {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
        {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
        {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
        {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
        {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
        {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
        {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
        {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
        {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
        {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
        {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
        {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
        {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
        {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
        {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
        {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
        {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
        {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
        {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
        {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
        {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
        {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
        {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
        {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
        {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
        {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
        {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
        {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
        {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
        {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
        {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
        {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
        {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
        {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
        {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
        {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
        {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
        {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
        {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
        {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
        {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
        {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
        {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
        {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
        {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
        {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
        {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
        {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};
//----------------------------------------------------------------------------------------------------------------------

//fGetOffset finds the approximate point of intersection of the surface
// between two points with the values fValue1 and fValue2
GLfloat marchingCubes::fGetOffset(GLfloat fValue1, GLfloat fValue2, GLfloat fValueDesired)
{
        GLdouble fDelta = fValue2 - fValue1;

        if(fDelta == 0.0)
        {
                return 0.5;
        }
        return (fValueDesired - fValue1)/fDelta;
}
//----------------------------------------------------------------------------------------------------------------------


GLvoid marchingCubes::vNormalizeVector(GLvector &rfVectorResult, GLvector &rfVectorSource)
{
        GLfloat fOldLength;
        GLfloat fScale;

        fOldLength = sqrtf( (rfVectorSource.fX * rfVectorSource.fX) +
                            (rfVectorSource.fY * rfVectorSource.fY) +
                            (rfVectorSource.fZ * rfVectorSource.fZ) );

        if(fOldLength == 0.0)
        {
                rfVectorResult.fX = rfVectorSource.fX;
                rfVectorResult.fY = rfVectorSource.fY;
                rfVectorResult.fZ = rfVectorSource.fZ;
        }
        else
        {
                fScale = 1.0/fOldLength;
                rfVectorResult.fX = rfVectorSource.fX*fScale;
                rfVectorResult.fY = rfVectorSource.fY*fScale;
                rfVectorResult.fZ = rfVectorSource.fZ*fScale;
        }
}
//----------------------------------------------------------------------------------------------------------------------

//time to write my own function mother fucker!
GLfloat marchingCubes::matStackSample(GLfloat fX, GLfloat fY, GLfloat fZ){
    float xPos = fX*(m_samplePercent.first * (float)(m_matStackSizeX-1));
    float yPos = fZ*(m_samplePercent.second * (float)(m_matStackSizeY-1));
    xPos+= m_samplePos.first * (float)(m_matStackSizeX-1);
    yPos+= m_samplePos.second * (float)(m_matStackSizeY-1);
    //correct any rounding errors
    if(xPos>(m_matStackSizeX-1)){ std::cout<<xPos<<" "<<fX<<std::endl; xPos = (m_matStackSizeX-1); }
    if(yPos>(m_matStackSizeY-1)){ std::cout<<yPos<<" "<<fZ<<std::endl; yPos = (m_matStackSizeY-1); }

    float y0,y1;
    float radius = fStepSize;
    (fY-(radius)<0) ? y0 = 0 : y0 = fY-(radius);
    (fY+(radius)>1) ? y1 = 1 : y1 = fY+(radius);

    glm::vec2 kernalBoxMin(xPos-radius,yPos-radius);
    glm::vec2 kernalBoxMax(xPos+radius,yPos+radius);
//    std::cout<<"kernalBoxMin "<<kernalBoxMin.x<<","<<kernalBoxMin.y<<std::endl;
    float tempMatVol = 0;
    float matVolTotal = 0;
    float dimX,dimY;
    int floorMinX = floor(kernalBoxMin.x);
    int floorMinY = floor(kernalBoxMin.y);
    int ceilMinX = ceil(kernalBoxMin.x);
    int ceilMinY = ceil(kernalBoxMin.y);
    int floorMaxX = floor(kernalBoxMax.x);
    int floorMaxY = floor(kernalBoxMax.y);
    int ceilMaxX = ceil(kernalBoxMax.x);
    int ceilMaxY = ceil(kernalBoxMax.y);

    for(int x=floorMinX; x<ceilMaxX+1;x++)
    for(int y=floorMinY; y<ceilMaxY+1;y++){
        if(floorMinX==ceilMinX){
            dimX = kernalBoxMax.x - kernalBoxMin.x;
        }
        else if(x==floorMinX)
            dimX = (float)ceilMinX - kernalBoxMin.x;
        else if(x==ceilMaxX)
            dimX = kernalBoxMax.x - (float)floorMaxX;
        else
            dimX = 1.0/(float)m_matStackSizeX;
        if(floorMinY==ceilMaxY){
            dimY = kernalBoxMax.y - kernalBoxMin.y;
        }
        else if(y==floorMinY)
            dimY = (float)ceilMinY - kernalBoxMin.y;
        else if(y==ceilMaxY)
            dimY = kernalBoxMax.y - (float)floorMaxY;
        else
            dimY = 1.0/(float)m_matStackSizeY;

        if(dimX==0||dimY==0)
            std::cout<<"one is 0 fuck! "<<xPos<<" xmin "<<kernalBoxMin.x<<","<<kernalBoxMax.x<<std::endl;
        tempMatVol = m_matStackData[x][y].amountOfMatBetween(y0,y1);
        tempMatVol *= dimX * dimY;
        matVolTotal+= tempMatVol;
    }


//std::cout<<(2*(matVolTotal/((2.0*radius)*(2.0*radius)*(2.0*radius)))) -1.0<<std::endl;

//    float volumeMat = m_matStackData[(int)xPos][(int)yPos].amountOfMatBetween(y0,y1);
    return (2*(matVolTotal/((2.0*radius)*(2.0*radius)*(2.0*radius)))) -1.0;



//    int floorX,floorY,xNext,yNext;
//    floorX = floor(xPos);
//    floorY = floor(yPos);
//    (floorX+1>m_matStackSizeX-1) ? xNext = floorX: xNext = floorX+1;
//    (floorY+1>m_matStackSizeY-1) ? yNext = floorY: yNext = floorY+1;

//    float n1,n2,n3,n4;
//    terrainGen::terrainType materialType = m_matStackData[floorX][floorY].matTypeAt(fY);
//    switch(materialType){
//    case(terrainGen::AIR): n1= 1; break;
//    default: n1 = -1; break;
//    }
//    materialType = m_matStackData[xNext][floorY].matTypeAt(fY);
//    switch(materialType){
//    case(terrainGen::AIR): n2= 1; break;
//    default: n2 = -1; break;
//    }
//    materialType = m_matStackData[floorX][yNext].matTypeAt(fY);
//    switch(materialType){
//    case(terrainGen::AIR): n3= 1; break;
//    default: n3 = -1; break;
//    }
//    materialType = m_matStackData[xNext][yNext].matTypeAt(fY);
//    switch(materialType){
//    case(terrainGen::AIR): n4= 1; break;
//    default: n4 = -1; break;
//    }


//    //bilinear interpoloation
//    float result = n1*(xNext - xPos)*(yNext - yPos) + n2*(xPos - floorX)*(yNext - yPos) + n3*(xNext - xPos)*(yPos-floorY) + n4*(xPos-floorX)*(yPos-floorY);
//    if(result==0)
//        return 0;
//    if(result>0)
//        return 1;
//    else
//        return -1;


}
//----------------------------------------------------------------------------------------------------------------------
GLfloat marchingCubes::textureSample(GLfloat fX, GLfloat fY, GLfloat fZ){
    int pixelPosX = fX*(m_samplePercent.first * (m_heightMap.width()-1));
    int pixelPosZ = fZ*(m_samplePercent.second * (m_heightMap.height()-1));
    pixelPosX+= m_samplePos.first * (m_heightMap.width()-1);
    pixelPosZ+= m_samplePos.second * (m_heightMap.height()-1);

    if(pixelPosX <m_heightMap.width() && pixelPosZ<m_heightMap.height()){
        QColor data = m_heightMap.pixel(pixelPosX,pixelPosZ);
        float height = data.red()/255.0;

        if (fY>height)
                return 1;
        if (fY==height)
            return 0;
        if (fY<height)
            return -1;
    }
    else{
        return 1;
    }
    return 0;
}
//----------------------------------------------------------------------------------------------------------------------
GLfloat marchingCubes::sphereSample(GLfloat fX, GLfloat fY, GLfloat fZ){
    float result = ((fX-0.5)*(fX-0.5) + (fY-0.5)*(fY-0.5) + (fZ-0.5)*(fZ-0.5) - 0.25);
    //std::cerr<<"the result is "<<result<<std::endl;
    return result;

}
//----------------------------------------------------------------------------------------------------------------------
GLfloat marchingCubes::fSample(GLfloat fX, GLfloat fY, GLfloat fZ)
{
    switch(m_sampleMode){
    case(MCTEXTURE):
        return textureSample(fX,fY,fZ);
    break;
    case(MCSPHERE):
        return sphereSample(fX,fY,fZ);
    break;
    case(MC_2DMATSTACK):
        return matStackSample(fX,fY,fZ);
    break;
    default:
        return 0;
    break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
GLvoid marchingCubes::vGetMatStackNormal(GLvector &rfNormal, GLfloat fX, GLfloat fY, GLfloat fZ){
    int xPos = fX*(m_samplePercent.first * (m_matStackSizeX-1));
    int yPos = fZ*(m_samplePercent.second * (m_matStackSizeY-1));
    xPos+= m_samplePos.first * (m_matStackSizeX-1);
    yPos+= m_samplePos.second * (m_matStackSizeY-1);
    if(  (xPos>0&&yPos>0)  &&  (xPos<(m_matStackSizeX-1)&&(yPos<(m_matStackSizeY-1)))    ){
        glm::vec3 vMid,vUp,vL,vR,vDwn,v1,v2,n1,n2,n3,n4,finalNormal;
        float height;
        //center point
        height = m_matStackData[xPos][yPos].totalHeight();
        vMid = glm::vec3((float)fX,height,(float)fZ);
        //point above
        height = m_matStackData[xPos][yPos+1].totalHeight();
        vUp = glm::vec3((float)fX,height,(float)fZ+fStepSize);
        //point to the left
        height = m_matStackData[xPos-1][yPos].totalHeight();
        vL = glm::vec3((float)fX-fStepSize,height,(float)fZ);
        //point to the right
        height = m_matStackData[xPos+1][yPos].totalHeight();
        vR = glm::vec3((float)fX+fStepSize,height,(float)fZ);
        //point below
        height = m_matStackData[xPos][yPos-1].totalHeight();
        vDwn = glm::vec3((float)fX,height,(float)fZ-fStepSize);

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
        //becuase Im a scrub I worked them out upside down :(
        finalNormal*=-1;
        rfNormal.fX = finalNormal.x;
        rfNormal.fY = finalNormal.y;
        rfNormal.fZ = finalNormal.z;
    }
    else{
        rfNormal.fX = 0;
        rfNormal.fY = 1;
        rfNormal.fZ = 0;
    }
}
//----------------------------------------------------------------------------------------------------------------------
GLvoid marchingCubes::vGetTextureNormal(GLvector &rfNormal, GLfloat fX, GLfloat fY, GLfloat fZ){
    int xPos = fX*(m_samplePercent.first * (m_heightMap.width()-1));
    int zPos = fZ*(m_samplePercent.second * (m_heightMap.height()-1));
    xPos+= m_samplePos.first * (m_heightMap.width()-1);
    zPos+= m_samplePos.second * (m_heightMap.height()-1);
    if(  (xPos>0&&zPos>0)  &&  (xPos<(m_heightMap.width()-1)&&(zPos<(m_heightMap.height()-1)))    ){
        glm::vec3 vMid,vUp,vL,vR,vDwn,v1,v2,n1,n2,n3,n4,finalNormal;
        QColor pixelData;
        //center point
        pixelData = m_heightMap.pixel(xPos,zPos);
        vMid = glm::vec3((float)fX,(float)pixelData.red()/255.0,(float)fZ);
        //point above
        pixelData = m_heightMap.pixel(xPos,zPos+1);
        vUp = glm::vec3((float)fX,(float)pixelData.red()/255.0,(float)fZ+1);
        //point to the left
        pixelData = m_heightMap.pixel(xPos-1,zPos);
        vL = glm::vec3((float)fX-1,(float)pixelData.red()/255.0,(float)fZ);
        //point to the right
        pixelData = m_heightMap.pixel(xPos+1,zPos);
        vR = glm::vec3((float)fX+1,(float)pixelData.red()/255.0,(float)fZ);
        //point below
        pixelData = m_heightMap.pixel(xPos,zPos-1);
        vDwn = glm::vec3((float)fX,(float)pixelData.red()/255.0,(float)fZ-1);

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
        //becuase Im a scrub I worked them out upside down :(
        finalNormal*=-1;
        rfNormal.fX = finalNormal.x;
        rfNormal.fY = finalNormal.y;
        rfNormal.fZ = finalNormal.z;
    }
    else{
        rfNormal.fX = 0;
        rfNormal.fY = 1;
        rfNormal.fZ = 0;
    }
}
//----------------------------------------------------------------------------------------------------------------------
GLvoid marchingCubes::vDefaultGetNormal(GLvector &rfNormal, GLfloat fX, GLfloat fY, GLfloat fZ){
    rfNormal.fX = fSample(fX-0.01, fY, fZ) - fSample(fX+0.01, fY, fZ);
    rfNormal.fY = fSample(fX, fY-0.01, fZ) - fSample(fX, fY+0.01, fZ);
    rfNormal.fZ = fSample(fX, fY, fZ-0.01) - fSample(fX, fY, fZ+0.01);
    vNormalizeVector(rfNormal, rfNormal);

}
//----------------------------------------------------------------------------------------------------------------------
//vGetNormal() finds the gradient of the scalar field at a point
//This gradient can be used as a very accurate vertx normal for lighting calculations
GLvoid marchingCubes::vGetNormal(GLvector &rfNormal, GLfloat fX, GLfloat fY, GLfloat fZ)
{
    switch(m_sampleMode){
    case(MCTEXTURE):
        vGetTextureNormal(rfNormal,fX,fY,fZ);
    break;
    case(MCSPHERE):
        vDefaultGetNormal(rfNormal,fX,fY,fZ);
    break;
    case(MC_2DMATSTACK):
        vGetMatStackNormal(rfNormal,fX,fY,fZ);
    break;
    default:
        vDefaultGetNormal(rfNormal,fX,fY,fZ);
    break;
    }
}
//----------------------------------------------------------------------------------------------------------------------

//vMarchCube1 performs the Marching Cubes algorithm on a single cube
void marchingCubes::vMarchCube1(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fScale)
{
        extern GLint aiCubeEdgeFlags[256];
        extern GLint a2iTriangleConnectionTable[256][16];

        GLint iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        GLfloat fOffset;
        GLfloat afCubeValue[8];
        GLvector asEdgeVertex[12];
        GLvector asEdgeNorm[12];

        //Make a local copy of the values at the cube's corners
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                afCubeValue[iVertex] = fSample(fX + a2fVertexOffset[iVertex][0]*fScale,
                                                   fY + a2fVertexOffset[iVertex][1]*fScale,
                                                   fZ + a2fVertexOffset[iVertex][2]*fScale);
        }

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(afCubeValue[iVertexTest] <= fTargetValue)
                        iFlagIndex |= 1<<iVertexTest;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
                // kinda a hack cos I dont know how to solve this atm lulz
                return;
        }

        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 12; iEdge++)
        {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge))
                {
                        fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ],
                                                     afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTargetValue);

                        asEdgeVertex[iEdge].fX = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
                        asEdgeVertex[iEdge].fY = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
                        asEdgeVertex[iEdge].fZ = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

                        vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
                }
        }


        //Draw the triangles that were found.  There can be up to five per cube
        for(iTriangle = 0; iTriangle < 5; iTriangle++)
        {
                if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
                        break;

                for(iCorner = 0; iCorner < 3; iCorner++)
                {
                        iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];

//                        glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
//                        glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);

                        m_position.push_back(glm::vec3(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ));
                        //m_normals.push_back(glm::vec3(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ));
                        float xTexPos = 0, yTexPos = 0;
                        if(m_sampleMode = MC_2DMATSTACK){
                            xTexPos = m_samplePos.first + (asEdgeVertex[iVertex].fX * m_samplePercent.first);
                            yTexPos = m_samplePos.second + (asEdgeVertex[iVertex].fZ * m_samplePercent.second);
                            xTexPos*=10.0;
                            yTexPos*=10.0;

                        }
                        else{
                            xTexPos = m_samplePos.first + (asEdgeVertex[iVertex].fX * m_samplePercent.first);
                            yTexPos = m_samplePos.second + (asEdgeVertex[iVertex].fZ * m_samplePercent.second);
                        }
                        m_texCoords.push_back(glm::vec2(xTexPos,yTexPos));
                }
                glm::vec3 v1 = m_position[m_position.size()-1];
                glm::vec3 v2 = m_position[m_position.size()-2];
                glm::vec3 v3 = m_position[m_position.size()-3];
                glm::vec3 e1 = v2-v1;
                glm::vec3 e2 = v3-v1;
                glm::vec3 normal = glm::normalize(glm::cross(e1,e2));
                normal*=-1.0;
                m_normals.push_back(normal);
                m_normals.push_back(normal);
                m_normals.push_back(normal);
        }
}
//----------------------------------------------------------------------------------------------------------------------
//vMarchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
void marchingCubes::vMarchTetrahedron(GLvector *pasTetrahedronPosition, GLfloat *pafTetrahedronValue)
{
        extern GLint aiTetrahedronEdgeFlags[16];
        extern GLint a2iTetrahedronTriangles[16][7];

        GLint iEdge, iVert0, iVert1, iEdgeFlags, iTriangle, iCorner, iVertex, iFlagIndex = 0;
        GLfloat fOffset, fInvOffset;
        GLvector asEdgeVertex[6];
        GLvector asEdgeNorm[6];


        //Find which vertices are inside of the surface and which are outside
        for(iVertex = 0; iVertex < 4; iVertex++)
        {
                if(pafTetrahedronValue[iVertex] <= fTargetValue)
                        iFlagIndex |= 1<<iVertex;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiTetrahedronEdgeFlags[iFlagIndex];

        //If the tetrahedron is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
            // kinda a hack cos I dont know how to solve this atm lulz
            return;
        }
        //Find the point of intersection of the surface with each edge
        // Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 6; iEdge++)
        {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge))
                {
                        iVert0 = a2iTetrahedronEdgeConnection[iEdge][0];
                        iVert1 = a2iTetrahedronEdgeConnection[iEdge][1];
                        fOffset = fGetOffset(pafTetrahedronValue[iVert0], pafTetrahedronValue[iVert1], fTargetValue);
                        fInvOffset = 1.0 - fOffset;

                        asEdgeVertex[iEdge].fX = fInvOffset*pasTetrahedronPosition[iVert0].fX  +  fOffset*pasTetrahedronPosition[iVert1].fX;
                        asEdgeVertex[iEdge].fY = fInvOffset*pasTetrahedronPosition[iVert0].fY  +  fOffset*pasTetrahedronPosition[iVert1].fY;
                        asEdgeVertex[iEdge].fZ = fInvOffset*pasTetrahedronPosition[iVert0].fZ  +  fOffset*pasTetrahedronPosition[iVert1].fZ;

                        vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
                }
        }


        //Draw the triangles that were found.  There can be up to 2 per tetrahedron
        for(iTriangle = 0; iTriangle < 2; iTriangle++)
        {
                if(a2iTetrahedronTriangles[iFlagIndex][3*iTriangle] < 0)
                        break;

                for(iCorner = 0; iCorner < 3; iCorner++)
                {
                        iVertex = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+iCorner];

//                        glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
//                        glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);


                        m_position.push_back(glm::vec3(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ));
                        m_normals.push_back(glm::vec3(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ));

                        float xTexPos, yTexPos;
                        if(m_sampleMode = MC_2DMATSTACK){
                            xTexPos = m_samplePos.first + (asEdgeVertex[iVertex].fX * m_samplePercent.first);
                            yTexPos = m_samplePos.second + (asEdgeVertex[iVertex].fZ * m_samplePercent.second);
                        }
                        else{
                            xTexPos = m_samplePos.first + (asEdgeVertex[iVertex].fX * m_samplePercent.first );
                            yTexPos = m_samplePos.second + (asEdgeVertex[iVertex].fZ * m_samplePercent.second );
                        }
                        m_texCoords.push_back(glm::vec2(xTexPos,yTexPos));
                }
        }
}
//----------------------------------------------------------------------------------------------------------------------


//vMarchCube2 performs the Marching Tetrahedrons algorithm on a single cube by making six calls to vMarchTetrahedron
void marchingCubes::vMarchCube2(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fScale)
{
        GLint iVertex, iTetrahedron, iVertexInACube;
        GLvector asCubePosition[8];
        GLfloat  afCubeValue[8];
        GLvector asTetrahedronPosition[4];
        GLfloat  afTetrahedronValue[4];
        std::vector<glm::vec3> vertecies;
        std::vector<glm::vec3> verteciesTemp;

        //Make a local copy of the cube's corner positions
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                asCubePosition[iVertex].fX = fX + a2fVertexOffset[iVertex][0]*fScale;
                asCubePosition[iVertex].fY = fY + a2fVertexOffset[iVertex][1]*fScale;
                asCubePosition[iVertex].fZ = fZ + a2fVertexOffset[iVertex][2]*fScale;
        }

        //Make a local copy of the cube's corner values
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                afCubeValue[iVertex] = fSample(asCubePosition[iVertex].fX,
                                                   asCubePosition[iVertex].fY,
                                               asCubePosition[iVertex].fZ);
        }

        for(iTetrahedron = 0; iTetrahedron < 6; iTetrahedron++)
        {
                for(iVertex = 0; iVertex < 4; iVertex++)
                {
                        iVertexInACube = a2iTetrahedronsInACube[iTetrahedron][iVertex];
                        asTetrahedronPosition[iVertex].fX = asCubePosition[iVertexInACube].fX;
                        asTetrahedronPosition[iVertex].fY = asCubePosition[iVertexInACube].fY;
                        asTetrahedronPosition[iVertex].fZ = asCubePosition[iVertexInACube].fZ;
                        afTetrahedronValue[iVertex] = afCubeValue[iVertexInACube];
                }
                vMarchTetrahedron(asTetrahedronPosition, afTetrahedronValue);

        }


}
//----------------------------------------------------------------------------------------------------------------------

//vMarchingCubes iterates over the entire dataset, calling vMarchCube on each cube
void marchingCubes::vMarchingCubes()
{
    GLint iX, iY, iZ;
    std::vector<glm::vec3> vertecies;
    std::vector<glm::vec3> verteciesTemp;

    m_position.clear();
    m_normals.clear();
    m_texCoords.clear();
    for(iX = 0; iX < iDataSetSize; iX++)
    for(iY = 0; iY < iDataSetSize; iY++)
    for(iZ = 0; iZ < iDataSetSize; iZ++)
    {
            vMarchCube1(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize);
    }

    if(!m_allocatedData){
        allocateGeomtryData();
    }
    else{
        updateGeometry();
    }


}
//----------------------------------------------------------------------------------------------------------------------
void marchingCubes::vMarchingTetrahedrons(){
    GLint iX, iY, iZ;
    std::vector<glm::vec3> vertecies;
    std::vector<glm::vec3> verteciesTemp;

    m_position.clear();
    m_normals.clear();
    m_texCoords.clear();
    for(iX = 0; iX < iDataSetSize; iX++)
    for(iY = 0; iY < iDataSetSize; iY++)
    for(iZ = 0; iZ < iDataSetSize; iZ++)
    {
            vMarchCube2(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize);
    }

    if(!m_allocatedData){
        allocateGeomtryData();
    }
    else{
        updateGeometry();
    }

}
//----------------------------------------------------------------------------------------------------------------------

