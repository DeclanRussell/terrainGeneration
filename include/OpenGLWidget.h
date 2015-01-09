#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#ifdef linux
    #include <GL/glew.h>
#endif
#include <QGLWidget>
#include <QEvent>
#include <QResizeEvent>
#include <QMessageBox>
#include <Camera.h>
#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "marchingcubes.h"
#include "terraingen.h"
#include "Trees.h"
#include "Grass.h"
#include "grasshair.h"
#include "GeometryClipmap.h"
#include "Water.h"
#include "Skybox.h"
#include "grasshairclipmap.h"

class OpenGLWidget : public QGLWidget
{
    Q_OBJECT //must include to gain access to qt stuff

public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for our NGL drawing class
    /// @param [in] parent the parent window to the class
    //----------------------------------------------------------------------------------------------------------------------
    explicit OpenGLWidget(const QGLFormat _format, QWidget *_parent=0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor must close down and release OpenGL resources
    //----------------------------------------------------------------------------------------------------------------------
    ~OpenGLWidget();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the virtual initialize class is called once when the window is created and we have a valid GL context
    /// use this to setup any default GL stuff
    //----------------------------------------------------------------------------------------------------------------------
    void initializeGL();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we want to draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void paintGL();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief called to resize the window
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(const int _w, const int _h );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief loads the matrices to the shader
    //----------------------------------------------------------------------------------------------------------------------
    void loadMatricesToShader(ShaderProgram *_currentShader);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief mouse move
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent(QMouseEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief mouse press event from Q_object
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent(QMouseEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief mouse release event from Q_object
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent(QMouseEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief key press event from Q_object
    //----------------------------------------------------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a timer event function from the Q_object
    //----------------------------------------------------------------------------------------------------------------------
    void timerEvent(QTimerEvent *);//
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief wheel event
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent(QWheelEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our marching cubes solver
    //----------------------------------------------------------------------------------------------------------------------
    marchingCubes * m_marchingCubesObject;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our terrain generator;
    //----------------------------------------------------------------------------------------------------------------------
    terrainGen * m_terrainFactory;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our grass generator
    //----------------------------------------------------------------------------------------------------------------------
    Grass * m_grassFactory;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our grass generator for our climap
    //----------------------------------------------------------------------------------------------------------------------
    grassHairClipmap * m_grassHairClipmapFactory;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our grass hair generator
    //----------------------------------------------------------------------------------------------------------------------
    grassHair * m_grassHairFactory;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our geomtry clipmap module
    //----------------------------------------------------------------------------------------------------------------------
    GeometryClipmap *m_geometryClipmap;
    //----------------------------------------------------------------------------------------------------------------------

private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the center location of our meso terrain
    //----------------------------------------------------------------------------------------------------------------------
    std::pair<float,float> m_mesoCenter;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a boolean so we know if we want to thermal erode our terrain every update
    //----------------------------------------------------------------------------------------------------------------------
    bool m_thermalErode;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a member to hold handles to our 4 terrain textures
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_terrainTextures[4];
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a handle to our mud texture
    //----------------------------------------------------------------------------------------------------------------------
    Texture *m_mudTex;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a handle to our grass texture
    //----------------------------------------------------------------------------------------------------------------------
    Texture *m_grassTex;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a handle to our rock texture
    //----------------------------------------------------------------------------------------------------------------------
    Texture *m_rockTex;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a handle to our snow texture
    //----------------------------------------------------------------------------------------------------------------------
    Texture *m_snowTex;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a handle to our bark texture
    //----------------------------------------------------------------------------------------------------------------------
    Texture *m_barkTex;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a handle to our bark texture
    //----------------------------------------------------------------------------------------------------------------------
    Texture *m_leavesTex;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our Camera
    //----------------------------------------------------------------------------------------------------------------------
    Camera *m_cam;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Model matrix
    //----------------------------------------------------------------------------------------------------------------------
    glm::mat4 m_modelMatrix;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Normal Matrix
    //----------------------------------------------------------------------------------------------------------------------
    glm::mat3 m_normalMatrix;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Modelview Projection matrix
    //----------------------------------------------------------------------------------------------------------------------
    glm::mat4 m_modelViewProjectionMatrix;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Vertex Shader
    //----------------------------------------------------------------------------------------------------------------------
    Shader *m_vertexShader;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Fragment Shader
    //----------------------------------------------------------------------------------------------------------------------
    Shader *m_fragmentShader;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Model matrix location
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_modelLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Projection matrix location
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_projLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Modelview matrix location
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_modelViewLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Normal matrix location
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_normalLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ModelViewProjection matrix location
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_modelViewProjectionLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Height of the window
    //----------------------------------------------------------------------------------------------------------------------
    int m_height;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Width of the window
    //----------------------------------------------------------------------------------------------------------------------
    int m_width;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Mouse transforms
    //----------------------------------------------------------------------------------------------------------------------
    glm::mat4 m_mouseGlobalTX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief model pos
    //----------------------------------------------------------------------------------------------------------------------
    glm::vec3 m_modelPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Spin face x
    //----------------------------------------------------------------------------------------------------------------------
    float m_spinXFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Sping face y
    //----------------------------------------------------------------------------------------------------------------------
    float m_spinYFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief rotate bool
    //----------------------------------------------------------------------------------------------------------------------
    bool m_rotate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief translate bool
    //----------------------------------------------------------------------------------------------------------------------
    bool m_translate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief
    //----------------------------------------------------------------------------------------------------------------------
    int m_origX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief
    //----------------------------------------------------------------------------------------------------------------------
    int m_origY;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief
    //----------------------------------------------------------------------------------------------------------------------
    int m_origXPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief
    //----------------------------------------------------------------------------------------------------------------------
    int m_origYPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a bool to regen marching cubes if we have moved
    //----------------------------------------------------------------------------------------------------------------------
    bool m_moved;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Skybox object
    //----------------------------------------------------------------------------------------------------------------------
    Skybox *m_skybox;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Water object
    //----------------------------------------------------------------------------------------------------------------------
    Water *m_water;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Generate the FBO needed to render the water
    //----------------------------------------------------------------------------------------------------------------------
    void genFBOs();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Reflect texture FBO
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_reflectFB;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Refract texture FBO
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_refractFB;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Render the reflection scene to a texture
    //----------------------------------------------------------------------------------------------------------------------
    void renderReflections();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Render the refraction scene to a texture
    //----------------------------------------------------------------------------------------------------------------------
    void renderRefractions();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Controls whether grass is drawn
    //----------------------------------------------------------------------------------------------------------------------
    bool m_drawGrass;
    //----------------------------------------------------------------------------------------------------------------------
};

#endif // OPENGLWIDGET_H
