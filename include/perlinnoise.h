#ifndef PERLINNOISE_H
#define PERLINNOISE_H


#include <QImage>


/// @file perlinNoise.h
/// @class perlinNoise
/// @brief The following functions are from Perlin Noise gernation by Hugo Elias http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
/// @brief This is a small class that generates perlin noise and saves is as a texture

class perlinNoise
{
public:
    struct textureDim{
        int height;
        int width;
    };

    //----------------------------------------------------------------------------------------------------------------------
    perlinNoise();
    //----------------------------------------------------------------------------------------------------------------------
    QImage genPerlinNoise(float _size = 16, float _offset = 0, int _octaves = 8, float _scale = 1.0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a mutator to set the dimensions of the texture we wish to genterate
    //----------------------------------------------------------------------------------------------------------------------
    inline void setTextureDim(int _x, int _y){m_textureSize.width = _x; m_textureSize.height = _y;}
    //----------------------------------------------------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------------------------------------------------------
    float PerlinNoise(int _x, int _y, float _size, float _offset, int _octaves);
    //----------------------------------------------------------------------------------------------------------------------
    float interpolatedNoise(float _x, float _y);
    //----------------------------------------------------------------------------------------------------------------------
    float smoothNoise(float _x, float _y);
    //----------------------------------------------------------------------------------------------------------------------
    float Noise(int _x, int _y);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a simple linear interpolation function
    //----------------------------------------------------------------------------------------------------------------------
    inline float lerp(float _x, float _y, float _a){return  _x*(1-_a) + _y*_a;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a member to store our texture dimentions so that we know how big a texture to generate
    //----------------------------------------------------------------------------------------------------------------------
    textureDim m_textureSize;
    //----------------------------------------------------------------------------------------------------------------------
};

#endif // PERLINNOISE_H
