#include "Skybox.h"

#include <QGLWidget>
#include <glm/gtc/matrix_inverse.hpp>
#ifdef DARWIN
    #include <noise/noise.h>
#else
    #include <libnoise/noise.h>
#endif

#include <noiseutils.h>

Skybox::Skybox(){
    initSkybox();
    createShader();
}

Skybox::~Skybox(){
    // Delete skybox model
    delete m_model;
    // Delete skybox shader program
    delete m_shaderProgram;
    delete m_cloudTex;
}

void Skybox::initSkybox(){
    m_model = new Model("models/sphere.obj");

    m_timer.start();

}

GLuint Skybox::loadCubeMap(std::string _pathToFile){
    GLuint texID;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
    const char * suffixes[] = { "posx", "negx", "posy",
       "negy", "posz", "negz" };
    GLuint targets[] = {
       GL_TEXTURE_CUBE_MAP_POSITIVE_X,
       GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
       GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
       GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
       GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
       GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for (int i =0; i<6; i++){
       GLuint texture;
       glGenTextures(1, &texture);
       glBindTexture(GL_TEXTURE_2D, texture);
       std::string texName = std::string(_pathToFile) + "_" + suffixes[i] + ".png";

       QString name = QString::fromUtf8(texName.c_str());
       QImage *image = new QImage(name);
       QImage tex = QGLWidget::convertToGLFormat(*image);
       glTexImage2D(targets[i],0, GL_RGBA, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
    }

    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texID;
}

void Skybox::createShader(){
    m_shaderProgram = new ShaderProgram();
    m_vertShader = new Shader("shaders/skyboxVert.glsl", GL_VERTEX_SHADER);
    m_fragShader =  new Shader("shaders/skyboxFrag.glsl", GL_FRAGMENT_SHADER);
    m_shaderProgram->attachShader(m_vertShader);
    m_shaderProgram->attachShader(m_fragShader);
    m_shaderProgram->bindFragDataLocation(0, "fragColour");
    m_shaderProgram->link();
    m_shaderProgram->use();

    delete m_vertShader;
    delete m_fragShader;

    ilInit();

    glGenTextures(1, &m_texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
    const char * suffixes[] = { "posx", "negx", "posy",
                                "negy", "posz", "negz" };
    GLuint targets[] = {
       GL_TEXTURE_CUBE_MAP_POSITIVE_X,
       GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
       GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
       GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
       GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
       GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for (int i =0; i<6; i++){
       std::string texName = std::string("textures/skyMap") + "_" + suffixes[i] + ".png";
       ILuint image = ilGenImage();
       ilBindImage(image);
       ILboolean loadSuccess = ilLoadImage(texName.c_str());
       if(!loadSuccess){
          std::cerr<<"Failed to load image: "<<texName<<std::endl;
          ilBindImage(0);
          ilDeleteImage(image);
       }
       ILboolean convertSuccess = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

       if(!convertSuccess){
          std::cerr<<"Failed to convert image: "<<texName<<std::endl;
          ilBindImage(0);
          ilDeleteImage(image);
       }
       glTexImage2D(targets[i], 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
    }

    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

//    int height = 512;
//    int width = 512;

//    noise::module::Perlin perlin;

//    perlin.SetFrequency(10.0);
//    GLubyte *data = new GLubyte[width * height * 4];
//    double xRange = 1.0;
//    double yRange = 1.0;
//    double xFactor = xRange / width;
//    double yFactor = yRange / height;

//    for (int oct = 0; oct<4; oct++){
//        perlin.SetOctaveCount(oct+1);
//        for (int i = 0; i<width; i++){
//            for (int j = 0; j<height; j++){
//                double x = xFactor * i;
//                double y = yFactor * j;
//                double z = 0.0;
//                float val = 0.0f;
//                double a, b, c, d;
//                a = perlin.GetValue(x, y, z);
//                b = perlin.GetValue(x+xRange, y, z);
//                c = perlin.GetValue(x, y+yRange, z);
//                d = perlin.GetValue(x+xRange, y+yRange, z);
//                double xmix = 1.0 - x / xRange;
//                double ymix = 1.0 - y / yRange;
//                double x1 = glm::mix(a, b, xmix);
//                double x2 = glm::mix(c, d, ymix);
//                val = glm::mix(x1, x2, ymix);
//                // Scale roughly between 0 and 1
//                val = (val + 1.0f) * 0.5f;
//                // Clamp strictly between 0 and 1
//                val = val > 1.0 ? 1.0 :val;
//                val = val < 0.0 ? 0.0 :val;
//                // Store in texture
//                data [((j * width + i) * 4) + oct] = (GLubyte) (val * 255.0f);
//            }
//        }
//    }

//    glGenTextures(1, &m_cloudTexID);
//    glBindTexture(GL_TEXTURE_2D, m_cloudTexID);

//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//    delete [] data;

    noise::module::Perlin perlin;
    noise::utils::NoiseMap heightMap;
    noise::utils::NoiseMapBuilderSphere heightMapBuilder;
    heightMapBuilder.SetSourceModule (perlin);
    heightMapBuilder.SetDestNoiseMap (heightMap);
    heightMapBuilder.SetDestSize (512, 256);
    heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
    heightMapBuilder.Build ();

    noise::utils::RendererImage renderer;
    noise::utils::Image image;
    renderer.SetSourceNoiseMap (heightMap);
    renderer.SetDestImage (image);
    renderer.Render ();

    noise::utils::WriterBMP writer;
    writer.SetSourceImage (image);
    writer.SetDestFilename ("textures/clouds.bmp");
    writer.WriteDestFile ();

    m_cloudTex = new Texture("textures/clouds.bmp");

}

void Skybox::update(){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_cloudTex->getTextureID());

    GLuint cubeMapLoc = m_shaderProgram->getUniformLoc("cubeMapTex");
    glUniform1i(cubeMapLoc, 0);

    GLuint cloudLoc = m_shaderProgram->getUniformLoc("cloudTex");
    glUniform1i(cloudLoc, 1);

    // Time uniform
    GLuint timeLoc = m_shaderProgram->getUniformLoc("time");
    glUniform1f(timeLoc, m_timer.elapsed());
}

void Skybox::loadMatricesToShader(glm::mat4 _modelMatrix, glm::mat4 _viewMatrix, glm::mat4 _projectionMatrix){
    m_shaderProgram->use();
    GLuint MVPLoc = m_shaderProgram->getUniformLoc("modelViewProjectionMatrix");
    GLuint modelViewMatrixLoc = m_shaderProgram->getUniformLoc("modelViewMatrix");
    GLuint normaMatrixLoc = m_shaderProgram->getUniformLoc("normalMatrix");

    glm::mat4 modelViewMatrix =  _viewMatrix * _modelMatrix;
    glm::mat4 normalMatrix = glm::inverseTranspose(modelViewMatrix);
    glm::mat4 modelViewProjectionMatrix = _projectionMatrix * _viewMatrix * _modelMatrix;

    glUniformMatrix4fv(modelViewMatrixLoc, 1, false, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(normaMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(MVPLoc, 1, false, glm::value_ptr(modelViewProjectionMatrix));
}

void Skybox::render(){
    update();
    glBindVertexArray(m_model->getVAO());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, m_model->getNumVerts());
    glBindVertexArray(0);
}
