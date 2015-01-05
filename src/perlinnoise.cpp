#include "include/perlinnoise.h"
#include <math.h>
#include <QColor>

perlinNoise::perlinNoise()
{
    setTextureDim(64,64);
}

float perlinNoise::Noise(int _x, int _y){
    int num = _x + _y * 57;
    num = (num<<13) ^ num;
    return ( 1.0 - ( (num * (num * num * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float perlinNoise::smoothNoise(float _x, float _y){
    float corners = ( Noise(_x-1, _y-1)+Noise(_x+1, _y-1)+Noise(_x-1, _y+1)+Noise(_x+1, _y+1) ) / 16;
    float sides   = ( Noise(_x-1, _y)  +Noise(_x+1, _y)  +Noise(_x, _y-1)  +Noise(_x, _y+1) ) /  8;
    float center  =  Noise(_x, _y) / 4;
    return corners + sides + center;
}

float perlinNoise::interpolatedNoise(float _x, float _y){
    int integer_X    = (int)_x;
    float fractional_X = _x - integer_X;

    int integer_Y    = (int)_y;
    float fractional_Y = _y - integer_Y;

    float v1 = smoothNoise(integer_X,     integer_Y);
    float v2 = smoothNoise(integer_X + 1, integer_Y);
    float v3 = smoothNoise(integer_X,     integer_Y + 1);
    float v4 = smoothNoise(integer_X + 1, integer_Y + 1);

    float i1 = lerp(v1 , v2 , fractional_X);
    float i2 = lerp(v3 , v4 , fractional_X);

    return lerp(i1 , i2 , fractional_Y);
}

float perlinNoise::PerlinNoise(int _x, int _y, float _size, float _offset, int _octaves){
    float total = 0;
    float persistence = 1.0/2.0;
    float frequency = 0;
    float amplitude = 0;
    for(int i=0; i<_octaves;i++){
        frequency = pow(2,i);
        amplitude = pow(persistence,i);
        total = total + interpolatedNoise((_x+_offset)/_size*frequency,(_y+_offset)/_size*frequency) * amplitude;
    }
    return total;
}

QImage perlinNoise::genPerlinNoise(float _size, float _offset, int _octaves, float _scale){
    QImage perlinNoiseTexture = QImage(m_textureSize.width,m_textureSize.height,QImage::Format_RGB32);
    float perlinNoise;
    for (int i=0; i<m_textureSize.width; i++){
        for (int j=0; j<m_textureSize.height; j++){
            perlinNoise = ((PerlinNoise(i,j,_size,_offset,_octaves)+1)/2)*255.0;
            perlinNoise*=_scale;
            perlinNoiseTexture.setPixel(i,j,QColor(perlinNoise,perlinNoise,perlinNoise,1).rgba());
        }
    }
//    perlinNoiseTexture.save("perlinNoise","PNG");
    return perlinNoiseTexture;
}
