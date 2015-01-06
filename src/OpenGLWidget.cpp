#include <QGuiApplication>
#include "OpenGLWidget.h"
#include "perlinnoise.h"
#include <iostream>

#define pi 3.14159265f

const static float INCREMENT=0.01;

OpenGLWidget::OpenGLWidget(const QGLFormat _format, QWidget *_parent) : QGLWidget(_format,_parent){
    // set this widget to have the initial keyboard focus
    setFocus();
    setFocusPolicy( Qt::StrongFocus );
    // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
    m_rotate=false;
    // mouse rotation values set to 0
    m_spinXFace=0;
    m_spinYFace=0;
    // initialze our thermal erosion trigger
    m_thermalErode = false;
    //initialise our model pos
    m_modelPos = glm::vec3(0.0);
    //set the center location of our meso terrain
    m_mesoCenter.first = m_mesoCenter.second = 0;
    // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
    m_moved = false;
    this->resize(_parent->size());
}
//----------------------------------------------------------------------------------------------------------------------
OpenGLWidget::~OpenGLWidget(){
    delete m_cam;


}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::initializeGL(){
#ifdef LINUX
    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if(error != GLEW_OK){
        std::cerr<<"GLEW IS NOT OK!!! "<<std::endl;
    }
#endif

    glClearColor(0.4f, 0.4f, 0.7f, 1.0f);
    // enable depth testing for drawing
    glEnable(GL_DEPTH_TEST);
    // enable multisampling for smoother drawing
    //glEnable(GL_MULTISAMPLE);

    // as re-size is not explicitly called we need to do this.
    glViewport(0,0,width(),height());


    // Initialise the model matrix
    m_modelMatrix = glm::mat4(1.0);

    // Initialize the camera
    glm::vec3 pos(0.0, 0.5, 0.5);
    m_cam = new Camera(pos);

    //create our volumetric data
    m_terrainFactory = new terrainGen(128,128);
    m_terrainFactory->addLayerFromTexture(QImage("textures/tnd"),terrainGen::BEDROCK);

    // Create Geometry
    m_geometryClipmap = new GeometryClipmap(3);
    m_geometryClipmap->setCutout(true);

    // Create plane for water
    m_water = new Water();

    m_marchingCubesObject = new marchingCubes();

    m_marchingCubesObject->setMode(marchingCubes::MC_2DMATSTACK);
    //std::cout<<m_terrainFactory->getSizeX()<<" "<<m_terrainFactory->getSizeY()<<std::endl;
    m_marchingCubesObject->setMatStack(m_terrainFactory->getData(),m_terrainFactory->getSizeX(),m_terrainFactory->getSizeY());
    m_marchingCubesObject->setSampleResolution(32);

    m_marchingCubesObject->setSampleSize(0.0625,0.0625);
    m_marchingCubesObject->setSamplePos(0.5-(0.0625/2.0),0.5-(0.0625/2.0));
    m_marchingCubesObject->vMarchingCubes();
//    m_marchingCubesObject->exportGeometryObj("middleTerrain");

    m_grassHairFactory = new grassHair(m_marchingCubesObject->getVAO());
    m_grassHairFactory->setGrassSize(0.05);
    m_grassHairFactory->setMaxGrassHeight(0.5);
    m_grassHairFactory->setMinGrassHeight(0.4);
    m_grassHairFactory->setMaxGrassAngle(30.0);
    m_grassHairFactory->setNumStrandsPerFace(3);


    glEnable(GL_DEPTH_TEST); // for removal of hidden surfaces


    startTimer(0);
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::createShader(){
    // Create a shader program
    m_shaderProgram = new ShaderProgram();
    m_vertexShader = new Shader("shaders/terrainVert.glsl", GL_VERTEX_SHADER);
    m_fragmentShader = new Shader("shaders/terrainFrag.glsl", GL_FRAGMENT_SHADER);

    m_shaderProgram->attachShader(m_vertexShader);
    m_shaderProgram->attachShader(m_fragmentShader);
    m_shaderProgram->bindFragDataLocation(0, "fragColour");
    m_shaderProgram->link();
    m_shaderProgram->use();

    delete m_vertexShader;
    delete m_fragmentShader;

    m_projLoc = m_shaderProgram->getUniformLoc("projectionMatrix");
    m_normalLoc = m_shaderProgram->getUniformLoc("normalMatrix");
    m_modelViewProjectionLoc = m_shaderProgram->getUniformLoc("modelViewProjectionMatrix");

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
    glUniform3f(ksLoc, 1.0, 1.0, 1.0);
    glUniform1f(shininessLoc, 100.0);

    // Set our mesh colouring information based on texture splatting
    GLuint mudHeight = m_shaderProgram->getUniformLoc("mudHeight");
    GLuint grassHeight = m_shaderProgram->getUniformLoc("grassHeight");
    GLuint rockHeight = m_shaderProgram->getUniformLoc("rockHeight");
    GLuint mudColour = m_shaderProgram->getUniformLoc("mudColour");
    GLuint grassColour = m_shaderProgram->getUniformLoc("grassColour");
    GLuint rockColour = m_shaderProgram->getUniformLoc("rockColour");
    GLuint snowColour = m_shaderProgram->getUniformLoc("snowColour");

    //set our height bands
    glUniform1f(mudHeight,0.1);
    glUniform1f(grassHeight,0.3);
    glUniform1f(rockHeight,0.6);

    //set the colours of our materials
    glUniform4f(mudColour, 0.0, 0.5, 0.0,1.0);
    glUniform4f(grassColour, 0.0, 0.5, 0.0,1.0);
    glUniform4f(rockColour, 0.5, 0.5, 0.5,1.0);
    glUniform4f(snowColour, 1.0, 1.0, 1.0,1.0);

}

//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::createTreeShader(){
    // Create a shader program
    m_treeShader = new ShaderProgram();
    m_vertexShader = new Shader("shaders/TreeVert.glsl", GL_VERTEX_SHADER);
    m_fragmentShader = new Shader("shaders/TreeFrag.glsl", GL_FRAGMENT_SHADER);

    m_treeShader->attachShader(m_vertexShader);
    m_treeShader->attachShader(m_fragmentShader);
    m_treeShader->bindFragDataLocation(0, "fragColour");
    m_treeShader->link();
    m_treeShader->use();

    delete m_vertexShader;
    delete m_fragmentShader;

    //send our tree textures to the gpu
    m_barkTex = new Texture("textures/bark1.tga");
    m_barkTex->bind(4);

    m_leavesTex = new Texture("textures/leafs1.tga");
    m_leavesTex->bind(5);

    //get our uniform locations
    GLuint lightPosTreeLoc = m_treeShader->getUniformLoc("light.position");
    GLuint lightIntTreeLoc = m_treeShader->getUniformLoc("light.intensity");
    GLuint kdTreeLoc = m_treeShader->getUniformLoc("Kd");
    GLuint kaTreeLoc = m_treeShader->getUniformLoc("Ka");
    GLuint ksTreeLoc = m_treeShader->getUniformLoc("Ks");
    GLuint shininessTreeLoc = m_treeShader->getUniformLoc("shininess");
    GLuint barkTreeLoc = m_treeShader->getUniformLoc("bark");
    GLuint leavesTreeLoc = m_treeShader->getUniformLoc("leaves");

    //set our uniforms
    glm::vec4 lightPositon(1.0, -3.0, 1.0, 1.0);
    glm::vec3 lightIntensity(1.0, 1.0, 1.0);
    glUniform4f(lightPosTreeLoc,lightPositon.x, lightPositon.y, lightPositon.z, lightPositon.w);
    glUniform3f(lightIntTreeLoc, lightIntensity.x, lightIntensity.y, lightIntensity.z);
    glUniform3f(kdTreeLoc, 0.5, 0.5, 0.5);
    glUniform3f(kaTreeLoc, 0.5, 0.5, 0.5);
    glUniform3f(ksTreeLoc, 1.0, 1.0, 1.0);
    glUniform1f(shininessTreeLoc, 10.0);
    //texture uniform positions
//    std::cout<<"bark handle location "<<barkTreeLoc<<std::endl;
//    std::cout<<"leaves handle location "<<leavesTreeLoc<<std::endl;
    glUniform1i(barkTreeLoc, 4);
    glUniform1i(leavesTreeLoc, 5);
}


//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::resizeGL(const int _w, const int _h){
    // set the viewport for openGL
    glViewport(0,0,_w,_h);
    m_cam->setShape(_w, _h);

}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::timerEvent(QTimerEvent *){
    if(m_thermalErode){
        m_terrainFactory->thermalErode();
        m_marchingCubesObject->vMarchingCubes();
        m_marchingCubesObject->exportGeometryObj("exporterTest");
    }
    updateGL();
}

//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Initialise the model matrix

    glm::mat4 rotx;
    glm::mat4 roty;

    rotx = glm::rotate(rotx, m_spinXFace, glm::vec3(1.0, 0.0, 0.0));
    roty = glm::rotate(roty, m_spinYFace, glm::vec3(0.0, 1.0, 0.0));

    m_mouseGlobalTX = rotx*roty;
    m_mouseGlobalTX = glm::translate(m_mouseGlobalTX, glm::vec3(0.0, m_modelPos.y, 0.0));


    glm::mat4 mesoModelMat = m_mouseGlobalTX;


//    bool reGenMesoTerrain = false;
//    if(m_modelPos.x<m_mesoCenter.first-0.05){
//        m_mesoCenter.first-=0.1;
//        reGenMesoTerrain = true;
//    }
//    if(m_modelPos.x>m_mesoCenter.first+0.05){
//        m_mesoCenter.first+=0.1;
//        reGenMesoTerrain = true;
//    }
//    if(m_modelPos.z<m_mesoCenter.second-0.05){
//        m_mesoCenter.second-=0.1;
//        reGenMesoTerrain = true;
//    }
//    if(m_modelPos.z>m_mesoCenter.second+0.05){
//        m_mesoCenter.second+=0.1;
//        reGenMesoTerrain = true;
//    }
//    if(reGenMesoTerrain){
//        m_marchingCubesObject->setSamplePos(0.4375 - ((float)m_mesoCenter.first * 0.125),0.4375 - ((float)m_mesoCenter.second * 0.125));
//        m_marchingCubesObject->vMarchingCubes();
//    }


    if(m_moved){
        glm::vec3 trans = (m_modelPos*glm::vec3(10000.0,10000.0,-10000.0));
        trans/=(2*512*32);
        trans/= 32.0;
    //    std::cout<<"trans "<<trans.x<<","<<trans.z<<std::endl;
        m_marchingCubesObject->setSamplePos(0.505-(0.0625/2.0) - trans.x,0.505-(0.0625/2.0) + trans.z);
        m_marchingCubesObject->vMarchingCubes();
        m_moved = false;
    }
    mesoModelMat = glm::scale(mesoModelMat,glm::vec3(1.0,4.0,1.0));
    mesoModelMat = glm::translate(mesoModelMat,glm::vec3(-0.5 - m_mesoCenter.first,0.0,-0.5 - m_mesoCenter.second));


//    //draw out meso terrain
    m_marchingCubesObject->draw(mesoModelMat, m_cam);

    glm::mat4 macroModelMat = m_mouseGlobalTX;
    //now draw toby's geomtry clipmap
    macroModelMat = glm::scale(macroModelMat, glm::vec3(-1.0, 1.0, 1.0));
    macroModelMat = glm::rotate(macroModelMat, pi, glm::vec3(0.0,1.0,0.0));
    glm::vec3 camPos = m_modelPos*glm::vec3(10000.0,10000.0,-10000.0);
//    std::cout<<"camPos "<<camPos.x<<","<<camPos.y<<","<<camPos.z<<std::endl;
    m_geometryClipmap->setViewPos(m_modelPos*glm::vec3(10000.0,10000.0,-10000.0));
    m_geometryClipmap->loadMatricesToShader(macroModelMat, m_cam->getViewMatrix(), m_cam->getProjectionMatrix());
    m_geometryClipmap->setWireframe(false);
    m_geometryClipmap->setCutout(true);
    //m_geometryClipmap->setCutOutPos(glm::vec2(-m_modelPos.x,m_modelPos.z));
    m_geometryClipmap->render();

    //draw our grass
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_grassHairFactory->draw(mesoModelMat, m_cam, m_marchingCubesObject->m_position.size());
    // Draw toby's water plane
    glBlendFunc(GL_ONE, GL_ONE);
    m_modelMatrix = m_mouseGlobalTX;
    m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(0.0, 1.5, 0.0));
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(80.0, 80.0, 80.0));
    m_water->loadMatricesToShader(m_modelMatrix, m_cam->getViewMatrix(), m_cam->getProjectionMatrix());
//    m_water->render();
    glDisable(GL_BLEND);


}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.01f * diffy;
    m_spinYFace += (float) 0.01f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();

  }
        // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);

    m_origXPos=_event->x();
    m_origYPos=_event->y();

    glm::mat4 rotx;
    glm::mat4 roty;
    glm::vec3 forwards(0.0, 0.5, 0.5);
    glm::vec3 up(0.0,1.0,0.0);
    glm::vec4 strafe(glm::cross(up,forwards),0.0);
    glm::vec4 forwardVec4(forwards,1.0);
    glm::vec4 upVec4(up,1.0);
    glm::mat4 rotFinal;


    rotx = glm::rotate(rotx, (float)(-1.0*m_spinXFace), glm::vec3(1.0, 0.0, 0.0));
    roty = glm::rotate(roty, (float)(-1.0*m_spinYFace), glm::vec3(0.0, 1.0, 0.0));
    rotFinal = /*rotx */roty;

    forwards = glm::vec3(rotFinal * forwardVec4);
    strafe = rotFinal * strafe;
    forwards = glm::normalize(forwards);
    glm::vec3 normStrafe = glm::normalize(glm::vec3(strafe));

    forwards*= (INCREMENT * diffY);
    normStrafe*= (INCREMENT * diffX);

    m_modelPos.x += forwards.x + normStrafe.x;
    m_modelPos.z += forwards.z + normStrafe.z;
    m_moved = true;

   }
  else if(m_translate && _event->buttons() == Qt::MiddleButton){
      int diffY = (int)(_event->y() - m_origYPos);
      m_origYPos=_event->y();
      m_modelPos.y -= INCREMENT * diffY;

  }
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::mousePressEvent ( QMouseEvent * _event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }
  else if(_event->button() == Qt::MiddleButton){
    m_origYPos = _event->y();
    m_translate=true;
  }

}
void OpenGLWidget::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
  if(_event->button() == Qt::MiddleButton){
    m_translate=false;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::keyPressEvent(QKeyEvent *_event){
    glm::mat4 rotx;
    glm::mat4 roty;
    glm::vec3 forwards(0.0, 0.5, 0.5);
    glm::vec3 up(0.0,1.0,0.0);
    glm::vec4 strafe(glm::cross(up,forwards),1.0);
    glm::vec4 forwardVec4(forwards,1.0);
    glm::vec4 upVec4(up,1.0);
    glm::mat4 rotFinal;

    switch(_event->key()){
    case(Qt::Key_T):
        // toggle our erosion boolean
        (m_thermalErode) ? m_thermalErode = false : m_thermalErode = true;
        break;
        // escape key to quit
         case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
        // show full screen
        case Qt::Key_F : showFullScreen(); break;
        // show windowed
        case Qt::Key_N : showNormal(); break;
        //now for some camera controls
        //calculate our camera rotations
        case Qt::Key_W:

            rotx = glm::rotate(rotx, (float)(-1.0*m_spinXFace), glm::vec3(1.0, 0.0, 0.0));
            roty = glm::rotate(roty, (float)(-1.0*m_spinYFace), glm::vec3(0.0, 1.0, 0.0));
            rotFinal = rotx *roty;

            forwards = glm::vec3(rotFinal * forwardVec4);
            up = glm::vec3(rotFinal * upVec4);

            forwards*=0.1;
            m_modelPos.x+=forwards.x;
            m_modelPos.z+=forwards.z;
            m_moved = true;

        break;
        case Qt::Key_S:
            rotx = glm::rotate(rotx, (float)(-1.0*m_spinXFace), glm::vec3(1.0, 0.0, 0.0));
            roty = glm::rotate(roty, (float)(-1.0*m_spinYFace), glm::vec3(0.0, 1.0, 0.0));
            rotFinal = rotx *roty;

            forwards = glm::vec3(rotFinal * forwardVec4);
            forwards*=0.1;
            m_modelPos.x-=forwards.x;
            m_modelPos.z-=forwards.z;
            m_moved = true;
        break;
        case Qt::Key_A:
            rotx = glm::rotate(rotx, (float)(-1.0*m_spinXFace), glm::vec3(1.0, 0.0, 0.0));
            roty = glm::rotate(roty, (float)(-1.0*m_spinYFace), glm::vec3(0.0, 1.0, 0.0));
            rotFinal = rotx *roty;

            strafe = rotFinal * strafe;
            strafe*=0.1;

            m_modelPos.x+= strafe.x;
            m_modelPos.z+= strafe.z;
            m_moved = true;

        break;
        case Qt::Key_D:
            rotx = glm::rotate(rotx, (float)(-1.0*m_spinXFace), glm::vec3(1.0, 0.0, 0.0));
            roty = glm::rotate(roty, (float)(-1.0*m_spinYFace), glm::vec3(0.0, 1.0, 0.0));
            rotFinal = rotx *roty;

            strafe = rotFinal * strafe;
            strafe*=0.1;

            m_modelPos.x-= strafe.x;
            m_modelPos.z-= strafe.z;
            m_moved = true;
        break;

    }
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::loadMatricesToShader(ShaderProgram *_currentShader){

    // Calculate projection matrix
    int scale = 1;
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(scale,scale,scale));
    glm::mat4 projectionMatrix = m_cam->getProjectionMatrix();

    glm::mat4 modelViewMatrix = m_cam->getViewMatrix() * m_modelMatrix;

    m_normalMatrix = glm::mat3(modelViewMatrix);
    m_normalMatrix = glm::inverse(m_normalMatrix);
    m_normalMatrix = glm::transpose(m_normalMatrix);

    m_modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    _currentShader->use();
    // Find uniform locations
    m_modelLoc = _currentShader->getUniformLoc("modelMatrix");
    m_projLoc = _currentShader->getUniformLoc("projectionMatrix");
    m_normalLoc = _currentShader->getUniformLoc("normalMatrix");
    m_modelViewProjectionLoc = _currentShader->getUniformLoc("modelViewProjectionMatrix");

    //glUniformMatrix4fv(m_modelLoc, 1, false, glm::value_ptr(m_modelMatrix));
    glUniformMatrix4fv(m_modelViewLoc, 1, false, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(m_projLoc, 1, false, glm::value_ptr(projectionMatrix));
    glUniformMatrix3fv(m_normalLoc, 1, false, glm::value_ptr(m_normalMatrix));
    glUniformMatrix4fv(m_modelViewProjectionLoc, 1, false, glm::value_ptr(m_modelViewProjectionMatrix));



}

