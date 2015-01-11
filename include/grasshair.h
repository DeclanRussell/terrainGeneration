#ifndef GRASSHAIR_H
#define GRASSHAIR_H

#ifdef DARWIN
#include <GLFW/glfw3.h>
    #include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
    #include <GL/gl.h>
#endif

#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"

class grassHair
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default constructor
    //----------------------------------------------------------------------------------------------------------------------
    grassHair();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default destructor
    //----------------------------------------------------------------------------------------------------------------------
    ~grassHair();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief constructor that will also set the VAO we wish to use
    //----------------------------------------------------------------------------------------------------------------------
    grassHair(GLuint _VAO);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mutator for our VAO
    //----------------------------------------------------------------------------------------------------------------------
    inline void setVAO(GLuint _VAO){m_VAO = _VAO;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our draw function
    //----------------------------------------------------------------------------------------------------------------------
    void draw(glm::mat4 _modelMatrix, Camera *_cam, int _numPoints);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mumtator for our grass size
    //----------------------------------------------------------------------------------------------------------------------
    inline void setGrassSize(float _size){m_grassSize = _size;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mutator to set the height of our grass
    //----------------------------------------------------------------------------------------------------------------------
    inline void setGrassHeight(float _height){m_grassHeight = _height;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mumtator for our max grass height
    //----------------------------------------------------------------------------------------------------------------------
    inline void setMaxGrassHeight(float _height){m_maxHeight = _height;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mumtator for our min grass height
    //----------------------------------------------------------------------------------------------------------------------
    inline void setMinGrassHeight(float _height){m_minHeight = _height;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mumtator for our max grass angle
    //----------------------------------------------------------------------------------------------------------------------
    inline void setMaxGrassAngle(float _angle){m_maxAngle = _angle;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mutator for the number of stands to be generated per face
    //----------------------------------------------------------------------------------------------------------------------
    inline void setNumStrandsPerFace(int _num){m_numStrandsPerFace = _num;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief accessor to the number of strands being drawn per face
    //----------------------------------------------------------------------------------------------------------------------
    inline int getNumStrandsPerFace(){return m_numStrandsPerFace;}
    //----------------------------------------------------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief load our matricies to our shader
    //----------------------------------------------------------------------------------------------------------------------
    void loadMatricesToShader(glm::mat4 _modelMatrix, Camera *_cam);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function that will set up our shader
    //----------------------------------------------------------------------------------------------------------------------
    void createShader();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our geometry VAO
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_VAO;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our shader program
    //----------------------------------------------------------------------------------------------------------------------
    ShaderProgram *m_shaderProgram;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a scalar for the height of our grass
    //----------------------------------------------------------------------------------------------------------------------
    float m_grassHeight;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a scalar for the length of the grass we decide to grow
    //----------------------------------------------------------------------------------------------------------------------
    float m_grassSize;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the maximum height our grass can grow
    //----------------------------------------------------------------------------------------------------------------------
    float m_maxHeight;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the minimum height our grass can grow
    //----------------------------------------------------------------------------------------------------------------------
    float m_minHeight;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the maximum angle our grass can grow
    //----------------------------------------------------------------------------------------------------------------------
    float m_maxAngle;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the number of grass strands to be generated per face
    //----------------------------------------------------------------------------------------------------------------------
    int m_numStrandsPerFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our grass texture
    //----------------------------------------------------------------------------------------------------------------------
    Texture *m_grassTexture;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the location of our time uniform in our shader
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_timeLoc;
    //----------------------------------------------------------------------------------------------------------------------


};

#endif // GRASSHAIR_H
