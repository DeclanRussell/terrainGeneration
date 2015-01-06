#ifndef TERRAINGEN_H
#define TERRAINGEN_H


#include <vector>
#include <QImage>
#include "anl.h"
#include <QProgressBar>

class terrainGen
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief defult constructor, initializes our data structure
    //----------------------------------------------------------------------------------------------------------------------
    terrainGen(int _sizeX, int _sizeY);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief enumorator of the type of material
    //----------------------------------------------------------------------------------------------------------------------
    enum terrainType{BEDROCK,SAND,ROCK,AIR};
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a structure holding the properties of our element
    //----------------------------------------------------------------------------------------------------------------------
    typedef struct{
        float height;
        terrainType type;
    }data;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a structure to define voxel data, this is used in our remove floating terrain function
    //----------------------------------------------------------------------------------------------------------------------
    typedef struct {
        terrainType type;
        int arrayLoc;
    }voxelData;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a structure to represent each element of our data structure
    //----------------------------------------------------------------------------------------------------------------------
    typedef struct{
        std::vector<data> properties;
        float totalHeight(){
            float sum = 0.0;
            //ignore the last one, atm pressuming it will be air thus no point in calculating
            for(unsigned int i=0;i<properties.size()-1;i++){
                sum+=properties[i].height;
            }
            return sum;
        }
        terrainType matTypeAt(float _y){
            float sum=0.0;
            for(unsigned int i=0; i<properties.size();i++){
                sum+=properties[i].height;
                if(_y<sum)
                    return properties[i].type;
            }
            return AIR;
        }
        float surfaceHeightAt(float _y){
            float sum=0.0;
            for(unsigned int i=0; i<properties.size();i++){
                sum+=properties[i].height;
                if(_y<sum)
                    return sum;
            }
        }

        int arrayLocAt(float _y){
            float sum=0.0;
            for(unsigned int i=0; i<properties.size();i++){
                sum+=properties[i].height;
                if(_y<sum)
                    return i;
            }
        }
    }ElmT;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function that will generate a height map from our data structure
    //----------------------------------------------------------------------------------------------------------------------
    QImage createHeightMap(int _sizeX, int _sizeY);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function that generates our terrain from 3 dimentional noise created with the ANL library
    /// @brief the method used is adopted from http://accidentalnoise.sourceforge.net/minecraftworlds.html
    //----------------------------------------------------------------------------------------------------------------------
    void createTerrainFromNoise();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function that uses growing algoritms to remove floating terrain
    //----------------------------------------------------------------------------------------------------------------------
    void removeFloatingTerrain(int _sampleSize = 64);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief clears the data in our data structure
    //----------------------------------------------------------------------------------------------------------------------
    inline void clearData(){delete m_terrainData;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns a pointer to our data
    //----------------------------------------------------------------------------------------------------------------------
    inline ElmT ** getData(){return m_terrainData;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function to add a layer of material to our data set from information from a texture
    //----------------------------------------------------------------------------------------------------------------------
    void addLayerFromTexture(QImage _tex, terrainType _type, float _scale = 1.0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function to simulate thermal erotion on the entire of our terrain
    //----------------------------------------------------------------------------------------------------------------------
    void thermalErode();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function to simulate thermal erotion on one element of our terrain
    //----------------------------------------------------------------------------------------------------------------------
    void thermalErode(int _x, int _y);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns the size of the data structure in the x direction
    //----------------------------------------------------------------------------------------------------------------------
    inline int getSizeX(){return m_sizeX;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns the size of the data structure in the y direction
    //----------------------------------------------------------------------------------------------------------------------
    inline int getSizeY(){return m_sizeY;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function to sort the layers of our terrain such that sand and rocks abide by gravity
    //----------------------------------------------------------------------------------------------------------------------
    void sortElement(int _x, int _y);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief function to sort all the elements materials in out data structure
    //----------------------------------------------------------------------------------------------------------------------
    void sortAllElements();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief create a 2d texture from our noise function
    //----------------------------------------------------------------------------------------------------------------------
    QImage create2DNoise();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief mutator to set our progress bar
    //----------------------------------------------------------------------------------------------------------------------
    inline void setProgressBar(QProgressBar *_barPtr){m_progressBar = _barPtr; m_progressBarSet = true;}
    //----------------------------------------------------------------------------------------------------------------------
    inline void setCaveAttBias(float _bias){m_caveAttenuateBias = _bias;}
    inline void setCaveThreshold(float _threshold){m_caveThreshold = _threshold;}
    inline void setLowLandOctaves(int _octaves){m_lowLandOctaves = _octaves;}
    inline void setLowLandFreq(float _freq){m_lowLandFreq = _freq;}
    inline void setHighLandOctaves(int _octaves){m_highLandOctaves = _octaves;}
    inline void setHighLandFreq(float _freq){m_highLandFreq = _freq;}
    inline void setMountainOctaves(int _octaves){m_mountainOctaves = _octaves;}
    inline void setMountainFreq(float _freq){m_mountainFreq = _freq;}
    
private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a progress bar that will be updated when creating our terrain
    //----------------------------------------------------------------------------------------------------------------------
    QProgressBar *m_progressBar;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a bool so we know if a progress bar has been set
    //----------------------------------------------------------------------------------------------------------------------
    bool m_progressBarSet;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our cave attenuate bias
    //----------------------------------------------------------------------------------------------------------------------
    float m_caveAttenuateBias;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our cave threshold
    //----------------------------------------------------------------------------------------------------------------------
    float m_caveThreshold;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief num low land noise octaves
    //----------------------------------------------------------------------------------------------------------------------
    int m_lowLandOctaves;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief frequency of the low land noise
    //----------------------------------------------------------------------------------------------------------------------
    float m_lowLandFreq;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief num high land noise octaves
    //----------------------------------------------------------------------------------------------------------------------
    int m_highLandOctaves;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief frequency of the high land noise
    //----------------------------------------------------------------------------------------------------------------------
    float m_highLandFreq;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief num mountain noise octaves
    //----------------------------------------------------------------------------------------------------------------------
    int m_mountainOctaves;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief frequency of the mountain noise
    //----------------------------------------------------------------------------------------------------------------------
    float m_mountainFreq;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our comparison function to be used in std::sort
    //----------------------------------------------------------------------------------------------------------------------
    static bool compareByType(const data &a, const data &b);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the repose angle for eroding rock
    //----------------------------------------------------------------------------------------------------------------------
    float m_sandReposeAngle;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the repose angle for eroding rock
    //----------------------------------------------------------------------------------------------------------------------
    float m_rockReposeAngle;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our data structure
    //----------------------------------------------------------------------------------------------------------------------
    ElmT **m_terrainData;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the size of our data structure in the x direction
    //----------------------------------------------------------------------------------------------------------------------
    int m_sizeX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the size of our data structure in the y direction
    //----------------------------------------------------------------------------------------------------------------------
    int m_sizeY;
    //----------------------------------------------------------------------------------------------------------------------



};

#endif // TERRAINGEN_H
