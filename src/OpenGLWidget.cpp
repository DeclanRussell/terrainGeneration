#include <QGuiApplication>
#include "OpenGLWidget.h"
#include "perlinnoise.h"
#include <iostream>

#define pi 3.14159265f

const static float INCREMENT=0.01;
const static float ZOOM = 0.1;

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
    delete m_skybox;
    delete m_water;
    delete m_geometryClipmap;
    delete m_terrainFactory;
    delete m_marchingCubesObject;
    delete m_grassHairFactory;
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
    glEnable(GL_DEPTH_TEST );
    // enable multisampling for smoother drawing
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_CLIP_PLANE0);

    // as re-size is not explicitly called we need to do this.
    glViewport(0,0,width(),height());

    // Initialise the model matrix
    m_modelMatrix = glm::mat4(1.0);

    // Initialize the camera
    glm::vec3 pos(0.0, 5.0, 0.1);
    m_cam = new Camera(pos);

    //create our volumetric data
    m_terrainFactory = new terrainGen(128,128);
    m_terrainFactory->addLayerFromTexture(QImage("textures/myPerlinHeightmap"),terrainGen::BEDROCK);

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
    m_grassHairFactory->setMaxGrassHeight(0.45);
    m_grassHairFactory->setMinGrassHeight(0.38);
    m_grassHairFactory->setMaxGrassAngle(30.0);
    m_grassHairFactory->setNumStrandsPerFace(3);

    // Crate the skybox object
    m_skybox = new Skybox();

    glEnable(GL_DEPTH_TEST); // for removal of hidden surfaces

    genFBOs();

    startTimer(0);
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::genFBOs(){
    // Gen framebuffer
    glGenFramebuffers(1, &m_reflectFB);
    glBindFramebuffer(GL_FRAMEBUFFER, m_reflectFB);

    // Bind texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *(m_water->getReflectTex()), 0);

    // Set the targets for the fragment shader output
    GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBufs);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Gen framebuffer
    glGenFramebuffers(1, &m_refractFB);
    glBindFramebuffer(GL_FRAMEBUFFER, m_refractFB);

    // Bind texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,*(m_water->getRefractTex()), 0);

    // Set the targets for the fragment shader output
    glDrawBuffers(1, drawBufs);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::renderReflections(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set the viewpoint equal to the size of the texture
    glViewport(0, 0, 512, 512);

    glm::mat4 rotx;
    glm::mat4 roty;

    rotx = glm::rotate(rotx, m_spinXFace, glm::vec3(1.0, 0.0, 0.0));
    roty = glm::rotate(roty, m_spinYFace, glm::vec3(0.0, 1.0, 0.0));

    m_mouseGlobalTX = rotx*roty;
    m_mouseGlobalTX[3][1] = m_modelPos.y;

    //Render the skybox;
    glm::mat4 modelMatrix = glm::mat4(1.0);
    modelMatrix = m_mouseGlobalTX;
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -1.5, 0.0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 3.0, 0.0));
    modelMatrix = glm::scale(modelMatrix , glm::vec3(100.0, 100.0, 100.0));

    // Reflect the modelMatrix
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0, -1.0, 1.0));

    m_skybox->loadMatricesToShader(modelMatrix, m_cam->getViewMatrix(), m_cam->getProjectionMatrix());
    m_skybox->render();

    glm::mat4 macroModelMat = m_mouseGlobalTX;

    //now draw toby's geomtry clipmap
    macroModelMat = glm::scale(macroModelMat, glm::vec3(-1.0, 1.0, 1.0));
    macroModelMat = glm::rotate(macroModelMat, pi, glm::vec3(0.0,1.0,0.0));

    // Reflect the modelMatrix
    macroModelMat = glm::scale(macroModelMat, glm::vec3(1.0, -1.0, 1.0));
    macroModelMat = glm::translate(macroModelMat, glm::vec3(0.0, -3.0, 0.0));

    m_geometryClipmap->setViewPos(m_modelPos*glm::vec3(10000.0,10000.0,-10000.0));
    m_geometryClipmap->loadClippedMatricesToShader(macroModelMat, m_cam->getViewMatrix(), m_cam->getProjectionMatrix());
    m_geometryClipmap->setWireframe(false);
    m_geometryClipmap->setCutout(false);
    //m_geometryClipmap->setCutOutPos(glm::vec2(-m_modelPos.x,m_modelPos.z));
    m_geometryClipmap->render();

}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::renderRefractions(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, 512, 512);

    glm::mat4 rotx;
    glm::mat4 roty;

    rotx = glm::rotate(rotx, m_spinXFace, glm::vec3(1.0, 0.0, 0.0));
    roty = glm::rotate(roty, m_spinYFace, glm::vec3(0.0, 1.0, 0.0));

    m_mouseGlobalTX = rotx*roty;
    m_mouseGlobalTX[3][1] = m_modelPos.y;

    glm::mat4 modelMatrix = glm::mat4(1.0);
    modelMatrix = m_mouseGlobalTX;
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 3.0, 0.0));
    modelMatrix = glm::scale(modelMatrix , glm::vec3(100.0, 100.0, 100.0));
    m_skybox->loadMatricesToShader(modelMatrix, m_cam->getViewMatrix(), m_cam->getProjectionMatrix());
    m_skybox->render();

    glm::mat4 mesoModelMat = m_mouseGlobalTX;

    if(m_moved){
        glm::vec3 trans = (m_modelPos*glm::vec3(10000.0,10000.0,-10000.0));
        trans/=(2*512*32);
        trans/= 32.0;
        m_marchingCubesObject->setSamplePos(0.505-(0.0625/2.0) - trans.x,0.505-(0.0625/2.0) + trans.z);
        m_marchingCubesObject->vMarchingCubes();
        m_moved = false;
    }
    mesoModelMat = glm::scale(mesoModelMat,glm::vec3(2.0,4.0,2.0));
    mesoModelMat = glm::translate(mesoModelMat,glm::vec3(-0.5 - m_mesoCenter.first,0.0,-0.5 - m_mesoCenter.second));

    //draw our meso terrain
    m_marchingCubesObject->draw(mesoModelMat, m_cam);

    glm::mat4 macroModelMat = m_mouseGlobalTX;

    //now draw toby's geomtry clipmap
    macroModelMat = glm::scale(macroModelMat, glm::vec3(-1.0, 1.0, 1.0));
    macroModelMat = glm::rotate(macroModelMat, pi, glm::vec3(0.0,1.0,0.0));

    m_geometryClipmap->setViewPos(m_modelPos*glm::vec3(10000.0,10000.0,-10000.0));
    m_geometryClipmap->loadMatricesToShader(macroModelMat, m_cam->getViewMatrix(), m_cam->getProjectionMatrix());
    m_geometryClipmap->setWireframe(false);
    m_geometryClipmap->setCutout(false);
    //m_geometryClipmap->setCutOutPos(glm::vec2(-m_modelPos.x,m_modelPos.z));
    m_geometryClipmap->render();

    //draw our grass
    m_grassHairFactory->draw(mesoModelMat, m_cam, m_marchingCubesObject->m_position.size());

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
    glBindFramebuffer(GL_FRAMEBUFFER, m_reflectFB);
    renderReflections();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_refractFB);
    renderRefractions();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, width()*devicePixelRatio(), height()*devicePixelRatio());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Initialise the model matrix

    glm::mat4 rotx;
    glm::mat4 roty;

    rotx = glm::rotate(rotx, m_spinXFace, glm::vec3(1.0, 0.0, 0.0));
    roty = glm::rotate(roty, m_spinYFace, glm::vec3(0.0, 1.0, 0.0));

    m_mouseGlobalTX = rotx*roty;
//    m_mouseGlobalTX = glm::translate(m_mouseGlobalTX, glm::vec3(0.0, m_modelPos.y, 0.0));
    m_mouseGlobalTX[3][1] = m_modelPos.y;

    glm::mat4 mesoModelMat = m_mouseGlobalTX;

    if(m_moved){
        glm::vec3 trans = (m_modelPos*glm::vec3(10000.0,10000.0,-10000.0));
        trans/=(2*512*32);
        trans/= 32.0;
        m_marchingCubesObject->setSamplePos(0.505-(0.0625/2.0) - trans.x,0.506-(0.0625/2.0) + trans.z);
        m_marchingCubesObject->vMarchingCubes();
        m_moved = false;
    }
    mesoModelMat = glm::scale(mesoModelMat,glm::vec3(2.0,4.0,2.0));
    mesoModelMat = glm::translate(mesoModelMat,glm::vec3(-0.5,0.0,-0.5));

    //draw our meso terrain
    m_marchingCubesObject->draw(mesoModelMat, m_cam);

    glm::mat4 macroModelMat = m_mouseGlobalTX;

    //now draw toby's geomtry clipmap
    macroModelMat = glm::scale(macroModelMat, glm::vec3(-1.0, 1.0, 1.0));
    macroModelMat = glm::rotate(macroModelMat, pi, glm::vec3(0.0,1.0,0.0));

    m_geometryClipmap->setViewPos(m_modelPos*glm::vec3(10000.0,10000.0,-10000.0));
    m_geometryClipmap->loadMatricesToShader(macroModelMat, m_cam->getViewMatrix(), m_cam->getProjectionMatrix());
    m_geometryClipmap->setWireframe(false);
    m_geometryClipmap->setCutout(true);
    //m_geometryClipmap->setCutOutPos(glm::vec2(-m_modelPos.x,m_modelPos.z));
    m_geometryClipmap->render();

    //draw our grass
    m_grassHairFactory->draw(mesoModelMat, m_cam, m_marchingCubesObject->m_position.size());

    glm::mat4 modelMatrix = glm::mat4(1.0);
    modelMatrix = m_mouseGlobalTX;
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 3.0, 0.0));
    modelMatrix = glm::scale(modelMatrix , glm::vec3(100.0, 100.0, 100.0));
    m_skybox->loadMatricesToShader(modelMatrix, m_cam->getViewMatrix(), m_cam->getProjectionMatrix());
    m_skybox->render();

    modelMatrix = glm::mat4(1.0);
    modelMatrix = m_mouseGlobalTX;
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 1.5, 0.0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(8.0, 1.0, 8.0));
    m_water->loadMatricesToShader(modelMatrix, m_cam->getViewMatrix(), m_cam->getProjectionMatrix());
    m_water->render();
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
void OpenGLWidget::wheelEvent(QWheelEvent *_event){
    if(_event->delta() > 0)
    {
        m_modelPos.y+=ZOOM;
    }
    else if(_event->delta() <0 )
    {
        m_modelPos.y-=ZOOM;
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

