#include "terraingen.h"
#include <QColor>
#include <iostream>
#include <algorithm>
#include "math.h"
//----------------------------------------------------------------------------------------------------------------------
terrainGen::terrainGen(int _sizeX, int _sizeY)
{
    m_terrainData = new ElmT*[_sizeX];
    for(int x=0; x<_sizeX; x++){
        m_terrainData[x] = new ElmT[_sizeY];
    }
    m_sizeX = _sizeX;
    m_sizeY = _sizeY;
    m_rockReposeAngle = 20;
    m_sandReposeAngle = 10;
    m_caveAttenuateBias = 0.3;
    m_caveThreshold = 0.48;
    m_lowLandOctaves = 2;
    m_lowLandFreq = 0.25;
    m_highLandOctaves = 4;
    m_highLandFreq = 2.0;
    m_mountainOctaves = 8;
    m_mountainFreq = 1.0;
    m_progressBarSet = false;

}

terrainGen::~terrainGen(){
    delete m_terrainData;
}

//----------------------------------------------------------------------------------------------------------------------
void terrainGen::createTerrainFromNoise(){
    //make sure our data structure is clear
    delete(m_terrainData);
    m_terrainData = new ElmT*[m_sizeX];
    for(int x=0; x<m_sizeX; x++){
        m_terrainData[x] = new ElmT[m_sizeY];
    }
    // divide the heavens from the earth by creating a gradient from 0 to 1
    anl::CImplicitGradient ground_Gradient;
    ground_Gradient.setGradient(0.0,0.0,0.0,1.0);

    //set up our low land function
    anl::CImplicitFractal lowland_shape_fractal(anl::BILLOW, anl::GRADIENT, anl::QUINTIC);
    lowland_shape_fractal.setNumOctaves(m_lowLandOctaves);
    lowland_shape_fractal.setFrequency(m_lowLandFreq);

    anl::CImplicitAutoCorrect lowland_autocorrect;
    lowland_autocorrect.setSource(&lowland_shape_fractal);
    lowland_autocorrect.setRange(0.0,1.0);

    anl::CImplicitScaleOffset lowland_scale(0.125,-0.45);
    lowland_scale.setSource(&lowland_autocorrect);

    anl::CImplicitScaleDomain lowland_y_scale;
    lowland_y_scale.setSource(&lowland_scale);
    lowland_y_scale.setYScale(0.0);

    anl::CImplicitTranslateDomain lowland_terrain;
    lowland_terrain.setSource(&ground_Gradient);
    lowland_terrain.setYAxisSource(&lowland_y_scale);

    //set up our high land function
    anl::CImplicitFractal highland_shape_fractal(anl::FBM, anl::GRADIENT, anl::QUINTIC);
    highland_shape_fractal.setNumOctaves(m_highLandOctaves);
    highland_shape_fractal.setFrequency(m_highLandFreq);

    anl::CImplicitAutoCorrect highland_autocorrect;
    highland_autocorrect.setSource(&highland_shape_fractal);
    highland_autocorrect.setRange(-1.0,1.0);

    anl::CImplicitScaleOffset highland_scale(0.25,0.0);
    highland_scale.setSource(&highland_autocorrect);

    anl::CImplicitScaleDomain highland_y_scale;
    highland_y_scale.setSource(&highland_scale);
    highland_y_scale.setYScale(0.0);

    anl::CImplicitTranslateDomain highland_terrain;
    highland_terrain.setSource(&ground_Gradient);
    highland_terrain.setYAxisSource(&highland_y_scale);

    //set up our mountains function
    anl::CImplicitFractal mountain_shape_fractal(anl::RIDGEDMULTI, anl::GRADIENT, anl::QUINTIC);
    mountain_shape_fractal.setNumOctaves(m_mountainOctaves);
    mountain_shape_fractal.setFrequency(m_mountainFreq);

    anl::CImplicitAutoCorrect mountain_autocorrect;
    mountain_autocorrect.setSource(&mountain_shape_fractal);
    mountain_autocorrect.setRange(-1.0,1.0);

    anl::CImplicitScaleOffset mountain_scale(0.45,0.15);
    mountain_scale.setSource(&mountain_autocorrect);

    anl::CImplicitScaleDomain mountain_y_scale;
    mountain_y_scale.setSource(&mountain_scale);
    mountain_y_scale.setYScale(0.25);

    anl::CImplicitTranslateDomain mountain_terrain;
    mountain_terrain.setSource(&ground_Gradient);
    mountain_terrain.setYAxisSource(&mountain_y_scale);

    //combine our 3 terrain functions
    anl::CImplicitFractal terrain_type_fractal(anl::FBM, anl::GRADIENT, anl::QUINTIC);
    terrain_type_fractal.setNumOctaves(3);
    terrain_type_fractal.setFrequency(0.125);

    anl::CImplicitAutoCorrect terrain_autocorrect;
    terrain_autocorrect.setSource(&terrain_type_fractal);
    terrain_autocorrect.setRange(0.0,1.0);

    anl::CImplicitScaleDomain terrain_type_y_scale;
    terrain_type_y_scale.setSource(&terrain_autocorrect);
    terrain_type_y_scale.setYScale(0.0);

    anl::CImplicitCache terrain_type_cache;
    terrain_type_cache.setSource(&terrain_type_y_scale);

    anl::CImplicitSelect highland_mountain_select;
    highland_mountain_select.setLowSource(&highland_terrain);
    highland_mountain_select.setHighSource(&mountain_terrain);
    highland_mountain_select.setControlSource(&terrain_type_cache);
    highland_mountain_select.setThreshold(0.55);
    highland_mountain_select.setFalloff(0.2);

    anl::CImplicitSelect highland_lowland_select;
    highland_lowland_select.setLowSource(&lowland_terrain);
    highland_lowland_select.setHighSource(&highland_mountain_select);
    highland_lowland_select.setControlSource(&terrain_type_cache);
    highland_lowland_select.setThreshold(0.25);
    highland_lowland_select.setFalloff(0.15);

    anl::CImplicitCache highland_lowland_select_cache;
    highland_lowland_select_cache.setSource(&highland_lowland_select);

    anl::CImplicitSelect ground_select;
    ground_select.setLowSource(0.0);
    ground_select.setHighSource(1.0);
    ground_select.setThreshold(0.5);
    ground_select.setControlSource(&highland_lowland_select_cache);

    //now to create our caves
    anl::CImplicitBias cave_attenuate_bias(m_caveAttenuateBias);
    cave_attenuate_bias.setSource(&highland_lowland_select_cache);

    anl::CImplicitFractal cave_shape1(anl::RIDGEDMULTI,anl::GRADIENT,anl::QUINTIC);
    cave_shape1.setNumOctaves(1.0);
    cave_shape1.setFrequency(4);

    anl::CImplicitFractal cave_shape2(anl::RIDGEDMULTI,anl::GRADIENT,anl::QUINTIC);
    cave_shape2.setNumOctaves(1.0);
    cave_shape2.setFrequency(4);

    anl::CImplicitCombiner cave_shape_attenuate(anl::MULT);
    cave_shape_attenuate.setSource(0,&cave_shape1);
    cave_shape_attenuate.setSource(1,&cave_attenuate_bias);
    cave_shape_attenuate.setSource(2,&cave_shape2);

    anl::CImplicitFractal cave_perturb_fractal(anl::FBM, anl::GRADIENT, anl::QUINTIC);
    cave_perturb_fractal.setNumOctaves(6);
    cave_perturb_fractal.setFrequency(3);

    anl::CImplicitScaleOffset cave_perturb_scale(1.0,0.0);
    cave_perturb_scale.setSource(&cave_perturb_fractal);

    anl::CImplicitTranslateDomain cave_perturb;
    cave_perturb.setSource(&cave_shape_attenuate);
    cave_perturb.setXAxisSource(&cave_perturb_scale);

    anl::CImplicitSelect cave_select;
    cave_select.setLowSource(1.0);
    cave_select.setHighSource(0.0);
    cave_select.setControlSource(&cave_perturb);
    cave_select.setThreshold(m_caveThreshold);
    cave_select.setFalloff(0.0);

    //finally combine our caves with our terrain
    anl::CImplicitCombiner ground_cave_multiply(anl::MULT);
    ground_cave_multiply.setSource(0,&cave_select);
    ground_cave_multiply.setSource(1,&ground_select);


    //now sample our function and pack the data into our data structure
    int height = (m_sizeX+m_sizeY)/2;
    int layer;
    float stepSize = 1.0/height;

    terrainType currentType, sampleType;
    data material;
    if(m_progressBarSet){
        m_progressBar->setMinimum(0);
        m_progressBar->setMaximum(m_sizeX);
        m_progressBar->setValue(0);
        m_progressBar->setVisible(true);
    }
    for(int x=0; x<m_sizeX; x++){
    for(int y=0; y<m_sizeY; y++){
        //inititialize some variables
        if(ground_cave_multiply.get(x*stepSize-5,1.0,y*stepSize-5)>0){
            currentType=BEDROCK;
        }
        else{
            currentType=AIR;
        }

        material.height = stepSize;
        material.type = currentType;
        m_terrainData[x][y].properties.push_back(material);
        layer = 0;

        //run through all the height locations at x,y
        for(int z=0; z<height; z++){
            if(ground_cave_multiply.get(x*stepSize-5,0.65-(z*stepSize),y*stepSize-5)>0){
                sampleType=BEDROCK;
            }
            else{
                sampleType=AIR;
            }

            //if we have the same material as the previous sampled location add some more height to it
            if(sampleType==currentType){
                m_terrainData[x][y].properties[layer].height+=stepSize;
            }
            //otherwise push back a new material
            else{
                material.height = stepSize;
                material.type = sampleType;
                m_terrainData[x][y].properties.push_back(material);
                currentType = sampleType;
                layer++;
            }
        }
    }
        if(m_progressBarSet){
            m_progressBar->setValue(x);
        }
    }

    if(m_progressBarSet)
        m_progressBar->setVisible(false);


//    std::cout<<"the size of out vector at 0,0 is "<<m_terrainData[0][0].properties.size()<<std::endl;

//    anl::TArray2D<TVec4D<float> > img(m_sizeX,m_sizeY);
//    anl::TArray3D<TVec4D<float> > img2(125,125,125);

//    anl::mapRGBA3D(anl::SEAMLESS_NONE,img2,compose1,ranges);
//    TVec4D<float> vec = img2.get(120,120,120);
//    float x = vec.get_x();
//    std::cout<<"the value of x is "<<x<<std::endl;

//    anl::CRGBACompositeChannels compose1(anl::RGB);
//    anl::SMappingRanges ranges;

//    compose1.setRedSource(&ground_cave_multiply);
//    compose1.setGreenSource(&ground_cave_multiply);
//    compose1.setBlueSource(&ground_cave_multiply);
//    compose1.setAlphaSource(1.0);

//    anl::mapRGBA2D(anl::SEAMLESS_NONE,img,compose1,ranges,0);
//    anl::saveRGBAArray((char*)"cave_multi.tga",&img);

//    compose1.setRedSource(&lowland_terrain);
//    compose1.setGreenSource(&lowland_terrain);
//    compose1.setBlueSource(&lowland_terrain);
//    compose1.setAlphaSource(1.0);

//    anl::mapRGBA2D(anl::SEAMLESS_NONE,img,compose1,ranges,0);
//    anl::saveRGBAArray((char*)"lowLand.tga",&img);

//    compose1.setRedSource(&highland_terrain);
//    compose1.setGreenSource(&highland_terrain);
//    compose1.setBlueSource(&highland_terrain);
//    compose1.setAlphaSource(1.0);

//    anl::mapRGBA2D(anl::SEAMLESS_NONE,img,compose1,ranges,0);
//    anl::saveRGBAArray((char*)"HighLand.tga",&img);

//    compose1.setRedSource(&mountain_terrain);
//    compose1.setGreenSource(&mountain_terrain);
//    compose1.setBlueSource(&mountain_terrain);
//    compose1.setAlphaSource(1.0);

//    anl::mapRGBA2D(anl::SEAMLESS_NONE,img,compose1,ranges,0);
//    anl::saveRGBAArray((char*)"moutainLand.tga",&img);

//    compose1.setRedSource(&mountain_terrain);
//    compose1.setGreenSource(&mountain_terrain);
//    compose1.setBlueSource(&mountain_terrain);
//    compose1.setAlphaSource(1.0);

//    anl::mapRGBA2D(anl::SEAMLESS_NONE,img,compose1,ranges,0);
//    anl::saveRGBAArray((char*)"moutainLand.tga",&img);


//    compose1.setRedSource(&ground_select);
//    compose1.setGreenSource(&ground_select);
//    compose1.setBlueSource(&ground_select);
//    compose1.setAlphaSource(1.0);

//    anl::mapRGBA2D(anl::SEAMLESS_NONE,img,compose1,ranges,0);
//    anl::saveRGBAArray((char*)"ground_select.tga",&img);

//    compose1.setRedSource(&cave_shape1);
//    compose1.setGreenSource(&cave_shape1);
//    compose1.setBlueSource(&cave_shape1);
//    compose1.setAlphaSource(1.0);

//    anl::mapRGBA2D(anl::SEAMLESS_NONE,img,compose1,ranges,0);
//    anl::saveRGBAArray((char*)"cave_shape.tga",&img);

//    compose1.setRedSource(&cave_select);
//    compose1.setGreenSource(&cave_select);
//    compose1.setBlueSource(&cave_select);
//    compose1.setAlphaSource(1.0);

//    anl::mapRGBA2D(anl::SEAMLESS_NONE,img,compose1,ranges,0);
//    anl::saveRGBAArray((char*)"cave_shape.tga",&img);



}
//----------------------------------------------------------------------------------------------------------------------
QImage terrainGen::create2DNoise(){
    // divide the heavens from the earth by creating a gradient from 0 to 1
    anl::CImplicitGradient ground_Gradient;
    ground_Gradient.setGradient(0.0,0.0,0.0,1.0);

    //set up our low land function
    anl::CImplicitFractal lowland_shape_fractal(anl::BILLOW, anl::GRADIENT, anl::QUINTIC);
    lowland_shape_fractal.setNumOctaves(m_lowLandOctaves);
    lowland_shape_fractal.setFrequency(m_lowLandFreq);

    anl::CImplicitAutoCorrect lowland_autocorrect;
    lowland_autocorrect.setSource(&lowland_shape_fractal);
    lowland_autocorrect.setRange(0.0,1.0);

    anl::CImplicitScaleOffset lowland_scale(0.125,-0.45);
    lowland_scale.setSource(&lowland_autocorrect);

    anl::CImplicitScaleDomain lowland_y_scale;
    lowland_y_scale.setSource(&lowland_scale);
    lowland_y_scale.setYScale(0.0);

    anl::CImplicitTranslateDomain lowland_terrain;
    lowland_terrain.setSource(&ground_Gradient);
    lowland_terrain.setYAxisSource(&lowland_y_scale);

    //set up our high land function
    anl::CImplicitFractal highland_shape_fractal(anl::FBM, anl::GRADIENT, anl::QUINTIC);
    highland_shape_fractal.setNumOctaves(m_highLandOctaves);
    highland_shape_fractal.setFrequency(m_highLandFreq);

    anl::CImplicitAutoCorrect highland_autocorrect;
    highland_autocorrect.setSource(&highland_shape_fractal);
    highland_autocorrect.setRange(-1.0,1.0);

    anl::CImplicitScaleOffset highland_scale(0.25,0.0);
    highland_scale.setSource(&highland_autocorrect);

    anl::CImplicitScaleDomain highland_y_scale;
    highland_y_scale.setSource(&highland_scale);
    highland_y_scale.setYScale(0.0);

    anl::CImplicitTranslateDomain highland_terrain;
    highland_terrain.setSource(&ground_Gradient);
    highland_terrain.setYAxisSource(&highland_y_scale);

    //set up our mountains function
    anl::CImplicitFractal mountain_shape_fractal(anl::RIDGEDMULTI, anl::GRADIENT, anl::QUINTIC);
    mountain_shape_fractal.setNumOctaves(m_mountainOctaves);
    mountain_shape_fractal.setFrequency(m_mountainFreq);

    anl::CImplicitAutoCorrect mountain_autocorrect;
    mountain_autocorrect.setSource(&mountain_shape_fractal);
    mountain_autocorrect.setRange(-1.0,1.0);

    anl::CImplicitScaleOffset mountain_scale(0.45,0.15);
    mountain_scale.setSource(&mountain_autocorrect);

    anl::CImplicitScaleDomain mountain_y_scale;
    mountain_y_scale.setSource(&mountain_scale);
    mountain_y_scale.setYScale(0.25);

    anl::CImplicitTranslateDomain mountain_terrain;
    mountain_terrain.setSource(&ground_Gradient);
    mountain_terrain.setYAxisSource(&mountain_y_scale);

    //combine our 3 terrain functions
    anl::CImplicitFractal terrain_type_fractal(anl::FBM, anl::GRADIENT, anl::QUINTIC);
    terrain_type_fractal.setNumOctaves(3);
    terrain_type_fractal.setFrequency(0.125);

    anl::CImplicitAutoCorrect terrain_autocorrect;
    terrain_autocorrect.setSource(&terrain_type_fractal);
    terrain_autocorrect.setRange(0.0,1.0);

    anl::CImplicitScaleDomain terrain_type_y_scale;
    terrain_type_y_scale.setSource(&terrain_autocorrect);
    terrain_type_y_scale.setYScale(0.0);

    anl::CImplicitCache terrain_type_cache;
    terrain_type_cache.setSource(&terrain_type_y_scale);

    anl::CImplicitSelect highland_mountain_select;
    highland_mountain_select.setLowSource(&highland_terrain);
    highland_mountain_select.setHighSource(&mountain_terrain);
    highland_mountain_select.setControlSource(&terrain_type_cache);
    highland_mountain_select.setThreshold(0.55);
    highland_mountain_select.setFalloff(0.2);

    anl::CImplicitSelect highland_lowland_select;
    highland_lowland_select.setLowSource(&lowland_terrain);
    highland_lowland_select.setHighSource(&highland_mountain_select);
    highland_lowland_select.setControlSource(&terrain_type_cache);
    highland_lowland_select.setThreshold(0.25);
    highland_lowland_select.setFalloff(0.15);

    anl::CImplicitCache highland_lowland_select_cache;
    highland_lowland_select_cache.setSource(&highland_lowland_select);

    anl::CImplicitSelect ground_select;
    ground_select.setLowSource(0.0);
    ground_select.setHighSource(1.0);
    ground_select.setThreshold(0.5);
    ground_select.setControlSource(&highland_lowland_select_cache);

    //now to create our caves
    anl::CImplicitBias cave_attenuate_bias(m_caveAttenuateBias);
    cave_attenuate_bias.setSource(&highland_lowland_select_cache);

    anl::CImplicitFractal cave_shape1(anl::RIDGEDMULTI,anl::GRADIENT,anl::QUINTIC);
    cave_shape1.setNumOctaves(1.0);
    cave_shape1.setFrequency(4);

    anl::CImplicitFractal cave_shape2(anl::RIDGEDMULTI,anl::GRADIENT,anl::QUINTIC);
    cave_shape2.setNumOctaves(1.0);
    cave_shape2.setFrequency(4);

    anl::CImplicitCombiner cave_shape_attenuate(anl::MULT);
    cave_shape_attenuate.setSource(0,&cave_shape1);
    cave_shape_attenuate.setSource(1,&cave_attenuate_bias);
    cave_shape_attenuate.setSource(2,&cave_shape2);

    anl::CImplicitFractal cave_perturb_fractal(anl::FBM, anl::GRADIENT, anl::QUINTIC);
    cave_perturb_fractal.setNumOctaves(6);
    cave_perturb_fractal.setFrequency(3);

    anl::CImplicitScaleOffset cave_perturb_scale(1.0,0.0);
    cave_perturb_scale.setSource(&cave_perturb_fractal);

    anl::CImplicitTranslateDomain cave_perturb;
    cave_perturb.setSource(&cave_shape_attenuate);
    cave_perturb.setXAxisSource(&cave_perturb_scale);

    anl::CImplicitSelect cave_select;
    cave_select.setLowSource(1.0);
    cave_select.setHighSource(0.0);
    cave_select.setControlSource(&cave_perturb);
    cave_select.setThreshold(m_caveThreshold);
    cave_select.setFalloff(0.0);

    //finally combine our caves with our terrain
    anl::CImplicitCombiner ground_cave_multiply(anl::MULT);
    ground_cave_multiply.setSource(0,&cave_select);
    ground_cave_multiply.setSource(1,&ground_select);


    if(m_progressBarSet){
        m_progressBar->setMinimum(0);
        m_progressBar->setMaximum(m_sizeX);
        m_progressBar->setValue(0);
        m_progressBar->setVisible(true);
    }
    QImage tex(m_sizeX,m_sizeY,QImage::Format_RGB32);
    float colour;
    for(int x=0; x<m_sizeX;x++){
    for(int y=0; y<m_sizeY;y++){
        colour = ground_cave_multiply.get((float)x/m_sizeX,((float)y/m_sizeY)-0.25,0.5);
        colour*=255.0;
        tex.setPixel(x,y,QColor(colour,colour,colour).rgb());
    }
        if(m_progressBarSet){
            m_progressBar->setValue(x);
        }
    }
    if(m_progressBarSet)
        m_progressBar->setVisible(false);
    tex.save("sample2DTEx","PNG");
    std::cout<<"texture created"<<std::endl;
    return tex;

}

//----------------------------------------------------------------------------------------------------------------------
void terrainGen::addLayerFromTexture(QImage _tex, terrainType _type, float _scale){
    float stepSizeX = (float)_tex.width()/m_sizeX;
    float stepSizeY = (float)_tex.height()/m_sizeY;
    QColor texColour;
    data elmt;
    int xPos,yPos;
    for(int x=0;x<m_sizeX;x++){
        for(int y=0; y<m_sizeY; y++){
            xPos = (int)x*stepSizeX;
            yPos = (int)y*stepSizeY;
            texColour = _tex.pixel(xPos,yPos);
            elmt.height = (texColour.redF()+texColour.greenF()+texColour.blueF())/3.0;
            elmt.height*=_scale;
            elmt.type = _type;
            m_terrainData[x][y].properties.push_back(elmt);
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void terrainGen::thermalErode(){
    for(int x=0; x<m_sizeX; x++){
        for(int y=0; y<m_sizeY; y++){
            thermalErode(x,y);
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void terrainGen::thermalErode(int _x, int _y){
    std::pair<int,int> selectedNeightbour(0,0);
    float maxHeightDif, heightSum, neighbourHeight, currentHeight, heightDif,deltaS,deltaSI;
    int xPos,yPos;
    bool neightbourFound;
    terrainType currentType;
    for(unsigned int layerNo=0;layerNo<m_terrainData[_x][_y].properties.size();layerNo++){
        //initialize our variables
        currentType = m_terrainData[_x][_y].properties[layerNo].type;
        maxHeightDif = 0.0;
        heightSum = 0.0;
        neightbourFound = false;
        currentHeight = m_terrainData[_x][_y].properties[layerNo].height;
        //lets find our best suited neighbough
        if(currentType == SAND || currentType ==ROCK)
        for(int offsetX=-1; offsetX<2;offsetX++){
            for(int offsetY=-1; offsetY<2;offsetY++){
                //dont want to include our center element
                if((offsetX!=0)&&(offsetY!=0)){
                    xPos = _x+offsetX;
                    yPos = _y+offsetY;
//                                std::cout<<xPos<<" "<<yPos<<std::endl;
                    //if our position is actually in our array
                    if((xPos>=0&&xPos<m_sizeX)&&(yPos>=0&&yPos<m_sizeY)){
                        neighbourHeight = m_terrainData[xPos][yPos].properties[layerNo].height;
                        // if out neightbour is lower than our currnt we can consider it
//                                    std::cout<<"found neightbour"<<std::endl;
                        heightSum+=neighbourHeight;
                        if(neighbourHeight<currentHeight){
                            heightDif = currentHeight-neighbourHeight;
                            if(heightDif>maxHeightDif){
                                //add some viscosity to our erotion by adding a repose angle
                                if((currentType==ROCK && heightDif<((1.0/m_sizeX)*tan(m_rockReposeAngle))) || (currentType==SAND && heightDif<((1.0/m_sizeX)*tan(m_sandReposeAngle)))){
                                maxHeightDif = heightDif;
                                selectedNeightbour.first = xPos;
                                selectedNeightbour.second = yPos;
                                neightbourFound = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        //well that was all very dull and boring
        //now lets actually do some calcuations
        //if we have actually found some neightbours
        if(neightbourFound){
            deltaS = maxHeightDif/2.0;
            deltaSI = deltaS * (  (m_terrainData[selectedNeightbour.first][selectedNeightbour.second].properties[layerNo].height)  /  heightSum  );
            m_terrainData[_x][_y].properties[layerNo].height-=deltaSI;
            m_terrainData[selectedNeightbour.first][selectedNeightbour.second].properties[layerNo].height+=deltaSI;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
bool terrainGen::compareByType(const data &a, const data &b){
    if((a.type != BEDROCK) && (b.type != BEDROCK)){
        return a.type < b.type;
    }
    else{
        return false;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void terrainGen::sortElement(int _x, int _y){
    //sort the all the materials in our element
    std::sort(m_terrainData[_x][_y].properties.begin(),m_terrainData[_x][_y].properties.end(),compareByType);
    for(unsigned int i=0; i<m_terrainData[_x][_y].properties.size();i++){
        //remove any elements that materials that dont have anything in them
        if(m_terrainData[_x][_y].properties[i].height==0)
            m_terrainData[_x][_y].properties.erase(m_terrainData[_x][_y].properties.begin()+i);
    }
}
//----------------------------------------------------------------------------------------------------------------------
void terrainGen::sortAllElements(){
    for(int x=0;x<m_sizeX;x++){
        for(int y=0;y<m_sizeY;y++){
            sortElement(x,y);
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------
QImage terrainGen::createHeightMap(int _sizeX, int _sizeY){
    QImage img(_sizeX,_sizeY,QImage::Format_RGB32);
    float xPos,yPos;
    int floorX,floorY,xNext,yNext;
    float h1,h2,h3,h4;
    float colour;
    QColor finColour;
    for(int x=0; x<_sizeX; x++){
    for(int y=0; y<_sizeY; y++){
        xPos = ((float)x/(float)_sizeX);
        yPos = ((float)y/(float)_sizeY);
        xPos *= (float)m_sizeX;
        yPos *= (float)m_sizeY;
        floorX = floor(xPos);
        floorY = floor(yPos);
        (floorX+1>m_sizeX-1) ? xNext = floorX: xNext = floorX+1;
        (floorY+1>m_sizeY-1) ? yNext = floorY: yNext = floorY+1;

        h1 = m_terrainData[floorX][floorY].totalHeight();
        h2 = m_terrainData[xNext][floorY].totalHeight();
        h3 = m_terrainData[floorX][yNext].totalHeight();
        h4 = m_terrainData[xNext][yNext].totalHeight();

        //bilinear interpoloation
        colour = h1*(xNext - xPos)*(yNext - yPos) + h2*(xPos - floorX)*(yNext - yPos) + h3*(xNext - xPos)*(yPos-floorY) + h4*(xPos-floorX)*(yPos-floorY);
        colour *= 255.0;
        if(colour>255) colour = 255;
        finColour.setRgb((int)colour,(int)colour,(int)colour);
        img.setPixel(x,y, finColour.rgb());
    }
    }
    //img.save("binInterpImg.png","PNG");
    return img;
}

//----------------------------------------------------------------------------------------------------------------------

