#include "GeometryClipmap.h"
#include <QImage>
#include <QGLWidget>
#include <glm/gtc/matrix_inverse.hpp>
#include "noiseutils.h"
#include "Bmp.h"

int width=1024,height=1024;
float INCRIMENT = 0.01;

//------------------------------------------------------------------------------------------------------------------------------------------
GeometryClipmap::GeometryClipmap(int _levels): m_gridSize(32), m_wireframe(false), m_cutout(false), m_height(4), m_updating(false){
    m_levels = _levels;
    m_viewPos = glm::vec3(0.0, 0.0, 0.0);
    m_xvalue = 2;
    m_zvalue = 2;

    createShader();
    Initialise();
}
//------------------------------------------------------------------------------------------------------------------------------------------
GeometryClipmap::~GeometryClipmap(){
    delete m_shaderProgram;
    delete m_clippedProgram;
    delete m_heightmapTex;
    delete m_normalmapTex;
    delete m_colourTex;
    delete m_grassTex;
    delete m_mudTex;
    delete m_rockTex;
    delete m_snowTex;
}
//------------------------------------------------------------------------------------------------------------------------------------------
void GeometryClipmap::createShader(){
    // Initialise the model matrix
    m_modelMatrix = glm::mat4(1.0);

    // Create a shader program
    m_shaderProgram = new ShaderProgram();
    m_vertShader = new Shader("shaders/geometryClipmapsVert.glsl", GL_VERTEX_SHADER);
    m_fragShader = new Shader("shaders/geometryClipmapsFrag.glsl", GL_FRAGMENT_SHADER);

    m_shaderProgram->attachShader(m_vertShader);
    m_shaderProgram->attachShader(m_fragShader);
    m_shaderProgram->bindFragDataLocation(0, "fragColour");
    m_shaderProgram->link();
    m_shaderProgram->use();

    delete m_vertShader;
    delete m_fragShader;

    m_modelViewProjectionLoc = m_shaderProgram->getUniformLoc("MVP");
    m_modelViewLoc = m_shaderProgram->getUniformLoc("MV");

    m_cutoutLoc = m_shaderProgram->getUniformLoc("cutout");
    m_heightLoc = m_shaderProgram->getUniformLoc("heightScale");
    GLuint fogMinLoc = m_shaderProgram->getUniformLoc("fogMin");
    GLuint fogMaxLoc = m_shaderProgram->getUniformLoc("fogMax");

    GLuint mudHeightLoc = m_shaderProgram->getUniformLoc("mudHeight");
    GLuint grassHeightLoc = m_shaderProgram->getUniformLoc("grassHeight");
    GLuint rockHeightLoc = m_shaderProgram->getUniformLoc("rockHeight");


    glUniform1f(mudHeightLoc, 0.4);
    glUniform1f(grassHeightLoc, 0.5);
    glUniform1f(rockHeightLoc, 0.7);
    glUniform1f(fogMinLoc, 3.0);
    glUniform1f(fogMaxLoc, 4.0);

    //------------------------------------------------------------------------------------------------------------------------
    m_clippedProgram = new ShaderProgram();
    m_vertShader = new Shader("shaders/GeometryClippedVert.glsl", GL_VERTEX_SHADER);
    m_fragShader = new Shader("shaders/geometryClipmapsFrag.glsl", GL_FRAGMENT_SHADER);

    m_clippedProgram->attachShader(m_vertShader);
    m_clippedProgram->attachShader(m_fragShader);
    m_clippedProgram->bindFragDataLocation(0, "fragColour");
    m_clippedProgram->link();
    m_clippedProgram->use();

    delete m_vertShader;
    delete m_fragShader;

    m_clippedCutoutLoc = m_clippedProgram->getUniformLoc("cutout");
    m_clippedHeightLoc = m_clippedProgram->getUniformLoc("heightScale");
    fogMinLoc = m_clippedProgram->getUniformLoc("fogMin");
    fogMaxLoc = m_clippedProgram->getUniformLoc("fogMax");

    mudHeightLoc = m_clippedProgram->getUniformLoc("mudHeight");
    grassHeightLoc = m_clippedProgram->getUniformLoc("grassHeight");
    rockHeightLoc = m_clippedProgram->getUniformLoc("rockHeight");

    glUniform1f(mudHeightLoc, 0.4);
    glUniform1f(grassHeightLoc, 0.5);
    glUniform1f(rockHeightLoc, 0.7);
    glUniform1f(fogMinLoc, 3.0);
    glUniform1f(fogMaxLoc, 4.0);
    // driver info
    std::cout<<"GL_VERSION: "<<glGetString(GL_VERSION)<<std::endl;
    std::cout<<"GL_RENDERER: "<<glGetString(GL_RENDERER)<<std::endl;
    std::cout<<"GL_VENDOR: " <<glGetString(GL_VENDOR)<<std::endl;
    //std::cout<<"GLU_VERSION: "<<gluGetString(GLU_VERSION)<<std::endl;
}
//------------------------------------------------------------------------------------------------------------------------------------------
void GeometryClipmap::Initialise(){

    m_heightmapBuilder.SetSourceModule(m_perlin);
    m_heightmapBuilder.SetDestNoiseMap(m_noisemap);
    m_heightmapBuilder.SetDestSize(1024, 1024);
    m_heightmapBuilder.SetBounds(m_xvalue, m_xvalue+4.0, m_zvalue, m_zvalue+4.0);
    m_heightmapBuilder.Build();

    m_heightmap.SetSourceNoiseMap(m_noisemap);
    m_heightmap.SetDestImage(m_image);
    m_heightmap.Render();

    m_writer.SetSourceImage(m_image);
    m_writer.SetDestFilename("textures/myPerlinHeightmap.bmp");
    m_writer.WriteDestFile();

    m_heightmapTex = new Texture("textures/myPerlinHeightmap.bmp");
//    m_heightmapTex = new Texture("textures/tnd");
//    m_heightmapTex = new Texture("textures/declanHeightmap.jpg");

    noise::utils::RendererNormalMap normalMapRenderer;
    noise::utils::Image normalImage;
    normalImage.SetSize(1024, 1024);
    normalMapRenderer.SetSourceNoiseMap(m_noisemap);
    normalMapRenderer.SetDestImage(normalImage);

    normalMapRenderer.Render();

    noise::utils::WriterBMP normalMapWriter;
    normalMapWriter.SetSourceImage(normalImage);
    normalMapWriter.SetDestFilename("textures/myPerlinNormalMap.bmp");
    normalMapWriter.WriteDestFile();

    m_normalmapTex = new Texture("textures/myPerlinNormalMap.bmp");

    //utils::RendererImage colourMap;
    m_colourMap.SetSourceNoiseMap(m_noisemap);
    m_colourMap.SetDestImage(m_imageColour);
    m_colourMap.ClearGradient();
    m_colourMap.AddGradientPoint(-1.0000, utils::Color(  50,   50, 50, 255)); // deeps
    m_colourMap.AddGradientPoint(-0.2500, utils::Color(  110,   110, 110, 255)); // shallow
    m_colourMap.AddGradientPoint( 0.0000, utils::Color(  140, 140, 140, 255)); // shore
    m_colourMap.AddGradientPoint( 0.0625, utils::Color(234, 220,  194, 255)); // sand
    m_colourMap.AddGradientPoint( 0.1250, utils::Color( 79, 106,   53, 255)); // grass
    m_colourMap.AddGradientPoint( 0.3750, utils::Color(157, 127,   75, 255)); // dirt
    m_colourMap.AddGradientPoint( 0.7500, utils::Color(143, 135, 124, 255)); // rock
    m_colourMap.AddGradientPoint( 1.0000, utils::Color(255, 255, 255, 255)); // snow
    m_colourMap.EnableLight();
    m_colourMap.SetLightContrast(5.0);
    m_colourMap.SetLightBrightness(2.0);
    m_colourMap.Render();

    m_writerColour.SetSourceImage(m_imageColour);
    m_writerColour.SetDestFilename("textures/myPerlinColourmap.bmp");
    m_writerColour.WriteDestFile();

    m_colourTex = new Texture("textures/myPerlinColourmap.bmp");


    //-----------------------------------------------------------------------------------------------------------------------------------
    // GrassTexture
    m_grassTex = new Texture("textures/grassTexture");

    // MudTexture
    m_mudTex = new Texture("textures/mudTexture");

    // SnowTexture
    m_snowTex = new Texture("textures/snowTexture");

    // RockTexture
    m_rockTex = new Texture("textures/rockTexture");

    //-----------------------------------------------------------------------------------------------------------------------------------

    // Make vbo quad patch
    for (unsigned int j=0; j<m_gridSize+1; ++j) for (unsigned int i=0; i<m_gridSize+2; ++i){
        for (unsigned int k=0; k<((i==0) ? 2 : 1);++k){
            m_vert.push_back(glm::vec3(float(i)/m_gridSize, 0.0, float(j)/m_gridSize));

        }
        ++j;
        for (unsigned int k=0; k<((i==m_gridSize+1) ? 2 : 1); ++k){
            m_vert.push_back(glm::vec3(float(i)/m_gridSize, 0.0, float(j)/m_gridSize));
        }
        --j;
    }

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*m_vert.size(), &m_vert[0].x, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

}
//------------------------------------------------------------------------------------------------------------------------------------------
void GeometryClipmap::update(){

    // Select if you want a cutout
    glUniform1i(m_cutoutLoc, m_cutout);
    glUniform1i(m_clippedCutoutLoc, m_cutout);
    // Set height of terrain
    glUniform1f(m_heightLoc, m_height);
    glUniform1i(m_clippedHeightLoc, m_height);

    GLuint mapPosLoc = m_shaderProgram->getUniformLoc("map_position");
    glUniform4f(mapPosLoc, (-m_viewPos.x/float(2*512*m_gridSize)-4), 0, (-m_viewPos.z/float(2*512*m_gridSize))-4, 0);
    mapPosLoc = m_clippedProgram->getUniformLoc("map_position");
    glUniform4f(mapPosLoc, (-m_viewPos.x/float(2*512*m_gridSize)-4), 0, (-m_viewPos.z/float(2*512*m_gridSize))-4, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_heightmapTex->getTextureID());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_grassTex->getTextureID());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_rockTex->getTextureID());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_snowTex->getTextureID());
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_mudTex->getTextureID());
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_normalmapTex->getTextureID());
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, m_colourTex->getTextureID());

    m_geoTextureLoc = m_shaderProgram->getUniformLoc("geoTexture");
    glUniform1i(m_geoTextureLoc, 0);
    m_clippedGeoTextureLoc = m_clippedProgram->getUniformLoc("geoTexture");
    glUniform1i(m_clippedGeoTextureLoc, 0);

    GLuint texture2Loc = m_shaderProgram->getUniformLoc("grassTexture");
    glUniform1i(texture2Loc, 1);
    texture2Loc = m_clippedProgram->getUniformLoc("grassTexture");
    glUniform1i(texture2Loc, 1);

    GLuint textureRockLoc = m_shaderProgram->getUniformLoc("rockTexture");
    glUniform1i(textureRockLoc, 2);
    textureRockLoc = m_clippedProgram->getUniformLoc("rockTexture");
    glUniform1i(textureRockLoc, 2);

    GLuint textureSnowLoc = m_shaderProgram->getUniformLoc("snowTexture");
    glUniform1i(textureSnowLoc, 3);
    textureSnowLoc = m_clippedProgram->getUniformLoc("snowTexture");
    glUniform1i(textureSnowLoc, 3);

    GLuint textureMudLoc = m_shaderProgram->getUniformLoc("mudTexture");
    glUniform1i(textureMudLoc, 4);
    textureMudLoc = m_clippedProgram->getUniformLoc("mudTexture");
    glUniform1i(textureMudLoc, 4);

    m_normalGeoTexLoc = m_shaderProgram->getUniformLoc("normalMap");
    glUniform1i(m_normalGeoTexLoc, 5);
    m_clippedNormalGeoLoc = m_clippedProgram->getUniformLoc("normalMap");
    glUniform1i(m_clippedNormalGeoLoc, 5);

    m_colourLoc = m_shaderProgram->getUniformLoc("colourMap");
    glUniform1i(m_colourLoc, 6);
    m_clippedColourLoc = m_clippedProgram->getUniformLoc("colourMap");
    glUniform1i(m_clippedColourLoc, 6);

}
//------------------------------------------------------------------------------------------------------------------------------------------
void GeometryClipmap::loadMatricesToShader(glm::mat4 _modelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projectionMatrix){
    m_shaderProgram->use();
    GLuint normalMatrixLoc = m_shaderProgram->getUniformLoc("normalMatrix");
    GLuint modelLoc = m_shaderProgram->getUniformLoc("modelMatrix");
    glm::mat4 modelViewMatrix = _viewMatrix * _modelMatrix;
    glm::mat4 normalMatrix = glm::inverseTranspose(modelViewMatrix);
    glm::mat4 modelViewProjectionMatrix = _projectionMatrix * _viewMatrix * _modelMatrix;
    glUniformMatrix4fv(m_modelViewLoc, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(m_modelViewProjectionLoc, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
}
void GeometryClipmap::loadClippedMatricesToShader(glm::mat4 _modelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projectionMatrix){
    m_clippedProgram->use();

    GLuint modelViewLoc = m_clippedProgram->getUniformLoc("MV");
    GLuint MVPLoc = m_clippedProgram->getUniformLoc("MVP");
    GLuint normalMatrixLoc = m_clippedProgram->getUniformLoc("normalMatrix");
    GLuint modelLoc = m_clippedProgram->getUniformLoc("modelMatrix");

    glm::mat4 modelViewMatrix = _viewMatrix * _modelMatrix;
    glm::mat4 normalMatrix = glm::inverseTranspose(modelViewMatrix);
    glm::mat4 modelViewProjectionMatrix = _projectionMatrix * _viewMatrix * _modelMatrix;

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
    glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
}
void GeometryClipmap::render(){
    update();
    float sxy=2; //scale x/y
    for (int i=0; i<m_levels; ++i){
        float ox=(int(m_viewPos.x*(1<<i))&511)/float(512*m_gridSize);
        float oz=(int(m_viewPos.z*(1<<i))&511)/float(512*m_gridSize);


        glm::vec3 scale(sxy, sxy, sxy);
        GLuint scaleLoc = m_shaderProgram->getUniformLoc("scale");
        glUniform4f(scaleLoc, scale.x, scale.y, scale.z, 1.0);
        scaleLoc = m_clippedProgram->getUniformLoc("scale");
        glUniform4f(scaleLoc, scale.x, scale.y, scale.z, 1.0);

        GLuint cutOutPosLoc = m_shaderProgram->getUniformLoc("cutOutLoc");
        glUniform2f(cutOutPosLoc,m_cutOutPos.x,m_cutOutPos.y);
        cutOutPosLoc = m_clippedProgram->getUniformLoc("cutOutLoc");
        glUniform2f(cutOutPosLoc, m_cutOutPos.x, m_cutOutPos.y);

        for (int k=-2; k<2; ++k) {
            for (int j=-2; j<2; ++j){
                // Set texture for heightmap
                glUniform1i(m_geoTextureLoc, 0);

                if (i!=m_levels-1) if (k==-1 || k==0) if (j==-1||j==0) continue;

                glm::vec3 offset(ox+float(j), 0, oz+float(k));
                if (k>=0) offset.z-=1.0/float(m_gridSize); //adjust offset for proper overlapping
                if (j>=0) offset.x-=1.0/float(m_gridSize); // adjust offset for proper overlapping

                // render
                GLuint offsetLoc = m_shaderProgram->getUniformLoc("offset");
                glUniform4f(offsetLoc, offset.x, 0.0, offset.z, 0.0);
                offsetLoc = m_clippedProgram->getUniformLoc("offset");
                glUniform4f(offsetLoc, offset.x, 0.0, offset.z, 0.0);

                glBindVertexArray(m_VAO);
                if (m_wireframe){
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                else{
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
                glPointSize(20.0);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vert.size());
                glBindVertexArray(0);
            }
        }
        sxy*=0.5;
    }
}

