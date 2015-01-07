TARGET=terrainGen
OBJECTS_DIR=obj

# as I want to support 4.8 and 5 this will set a flag for some of the mac stuff
# mainly in the types.h file for the setMacVisual which is native in Qt5
isEqual(QT_MAJOR_VERSION, 5) {
        cache()
        DEFINES +=QT5BUILD
}
UI_HEADERS_DIR=ui
MOC_DIR=moc

CONFIG-=app_bundle
QT+=gui opengl core
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/Camera.cpp \
    src/ShaderUtils.cpp \
    src/TextureUtils.cpp \
    src/ShaderProgram.cpp \
    src/Texture.cpp \
    src/OpenGLWidget.cpp \
    src/Shader.cpp \
    src/marchingcubes.cpp \
    src/perlinnoise.cpp \
    src/terraingen.cpp \
    src/Grass.cpp \
    src/Trees.cpp \
    src/Model.cpp \
    src/grasshair.cpp \
    src/GeometryClipmap.cpp \
    src/noiseutils.cpp \
    src/Bmp.cpp \
    src/Skybox.cpp \
    src/Water.cpp

HEADERS += \
    include/mainwindow.h \
    include/Camera.h \
    include/ShaderUtils.h \
    include/TextureUtils.h \
    include/ShaderProgram.h \
    include/Texture.h \
    include/OpenGLWidget.h \
    include/Shader.h \
    include/marchingcubes.h \
    include/perlinnoise.h \
    include/terraingen.h \
    include/Trees.h \
    include/Grass.h \
    include/Model.h \
    include/grasshair.h \
    include/GeometryClipmap.h \
    include/noiseutils.h \
    include/Bmp.h \
    include/Skybox.h \
    include/Water.h

HEADERS+=accidentalnoise/include/*.h
#HEADERS-=accidentalnoise/include/*.h
SOURCES+=accidentalnoise/src/*.cpp
#SOURCES-=accidentalnoise/src/*.cpp

INCLUDEPATH +=./include /opt/local/include  /home/i7709212/customlibs/glm ./accidentalnoise/include /usr/local/include
LIBS += -L/opt/local/lib -lassimp -lnoise -lIL
linux:LIBS += -lGLEW
DESTDIR=./

CONFIG += console
CONFIG -= app_bundle

# use this to suppress some warning from boost
QMAKE_CXXFLAGS_WARN_ON += "-Wno-unused-parameter"
QMAKE_CXXFLAGS+= -msse -msse2 -msse3
macx:QMAKE_CXXFLAGS+= -arch x86_64
macx:INCLUDEPATH+=/usr/local/include/
# define the _DEBUG flag for the graphics lib

unix:LIBS += -L/usr/local/lib

# now if we are under unix and not on a Mac (i.e. linux) define GLEW
linux-*{
                linux-*:QMAKE_CXXFLAGS +=  -march=native
                linux-*:DEFINES+=GL42
                DEFINES += LINUX
}
DEPENDPATH+=include
# if we are on a mac define DARWIN
macx:DEFINES += DARWIN


FORMS += \
    ui/mainwindow.ui

OTHER_FILES += \
    shaders/PhongFrag.glsl \
    shaders/PhongVert.glsl \
    shaders/terrainFrag.glsl \
    shaders/terrainVert.glsl \
    shaders/textureTerrainFrag.glsl \
    shaders/textureTerrainVert.glsl \
    textures/grassTexture.jpg \
    textures/rockTexture.jpg \
    textures/snowTexture.jpg \
    textures/mudTexture.png \
    textures/bark1.tga \
    textures/leafs1.tga \
    textures/grassBlade.png \
    shaders/grassFrag.glsl \
    shaders/grassVert.glsl \
    shaders/TreeFrag.glsl \
    shaders/TreeVert.glsl \
    shaders/grassHairVert.glsl \
    shaders/grassHairGeom.glsl \
    shaders/grassHairFrag.glsl \
    shaders/geometryClipmapsFrag.glsl \
    shaders/geometryClipmapsVert.glsl \
    shaders/waterFrag.glsl \
    shaders/waterVert.glsl \
    textures/myPerlinHeightmap.bmp \
    shaders/skyboxFrag.glsl \
    shaders/skyboxVert.glsl \
    models/skyBox.obj \
    textures/tnd.jpg \
    models/water.obj \
    shaders/ripplesFrag.glsl \
    shaders/ripplesVert.glsl \
    textures/water_dudv.jpg \
    textures/water_normal.jpg \
    shaders/GeometryClippedVert.glsl
