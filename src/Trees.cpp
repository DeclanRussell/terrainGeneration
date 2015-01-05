#include "Trees.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXTREES 100
Trees::Trees(sampleMode _mode){
    m_numTrees = 0;
    m_minTreeHeight = 0.1;
    m_maxTreeHeight = 0.4;
}
//----------------------------------------------------------------------------------------------------------------------
Trees::~Trees(){
    if (!m_billboards){
        delete m_tree;
        delete m_leaves;
    }
    else{
        //delete m_billboard;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void Trees::createTrees(){
    // load model
    m_tree = new Model("models/tree.obj");
    m_leaves = new Model("models/leaves.obj");
    glBindVertexArray(m_tree->getVAO());

    // Instance  offset buffer
    GLfloat m_offset[m_numTrees*3];
    srand(time(NULL));

    // COULD BE SPED UP USING TRANSFORM FEEDBACK !!
    float x;
    float y;
    float z;
    for (int i=0; i<m_numTrees*3; i+=3){
        x = -(0.0) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((1.0)-(0.0))));
        z = -(0.0) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((1.0)-(0.0))));
        if(m_mode==T_HEIGHTMAP){
            y = getHeightFromMatStack((int)(x * m_heightmap.width()), (int)(z * m_heightmap.height()));
        }
        else{
            y = getHeightFromMatStack((int)(x * m_matStackSizeX-1),(int)(z * m_matStackSizeY-1));
        }

        while (y < m_minTreeHeight || y > m_maxTreeHeight){
            x = -(0.0) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((1.0)-(0.0))));
            z = -(0.0) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((1.0)-(0.0))));
            if(m_mode==T_HEIGHTMAP){
                y = getHeightFromHeightMap((int)(x * m_heightmap.width()), (int)(z * m_heightmap.height()));
            }
            else{
                y = getHeightFromMatStack((int)(x * m_matStackSizeX-1),(int)(z * m_matStackSizeY-1));
            }
        }

        m_offset[i] = x;
        m_offset[i+1] = y;
        m_offset[i+2] = z;
        m_positions.push_back(glm::vec2(x, z));

    }

    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_offset), &m_offset[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    glBindVertexArray(m_leaves->getVAO());
    GLuint leavesPositionBuffer;
    glGenBuffers(1, &leavesPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, leavesPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_offset), &m_offset[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &leavesPositionBuffer);
}
//----------------------------------------------------------------------------------------------------------------------
float Trees::getHeightFromHeightMap(int _x, int _y){
    QColor _pixel = m_heightmap.pixel(_x, _y);
    return ((_pixel.redF()+_pixel.blueF()+_pixel.greenF())/3.0)/4.0;
}
//----------------------------------------------------------------------------------------------------------------------
float Trees::getHeightFromMatStack(int _x, int _y){
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
void Trees::render(GLuint _isBarkLoc){

        glUniform1i(_isBarkLoc, true);
        glBindVertexArray(m_tree->getVAO());
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_tree->getNumVerts(), m_numTrees);
        glBindVertexArray(0);

        glUniform1i(_isBarkLoc, false);
        glBindVertexArray(m_leaves->getVAO());
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_leaves->getNumVerts(), m_numTrees);
        glBindVertexArray(0);

}
//----------------------------------------------------------------------------------------------------------------------
void Trees::updateTrees(QString _pathToHeightMap){
    QImage _heightMap(_pathToHeightMap);
    m_tree->deleteVAO();
    m_leaves->deleteVAO();
    m_positions.clear();
    createTrees();
}
//----------------------------------------------------------------------------------------------------------------------
