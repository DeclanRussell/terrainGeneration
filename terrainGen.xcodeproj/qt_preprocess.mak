#############################################################################
# Makefile for building: terrainGen
# Generated by qmake (2.01a) (Qt 4.8.3) on: Tue Jan 6 14:47:04 2015
# Project:  terrainGen.pro
# Template: app
# Command: /usr/bin/qmake -o terrainGen.xcodeproj/project.pbxproj terrainGen.pro
#############################################################################

MOC       = /Developer/Tools/Qt/moc
UIC       = /Developer/Tools/Qt/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -DDARWIN -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
INCPATH       = -I/usr/local/Qt4.8/mkspecs/macx-xcode -I. -I/Library/Frameworks/QtCore.framework/Versions/4/Headers -I/usr/include/QtCore -I/Library/Frameworks/QtGui.framework/Versions/4/Headers -I/usr/include/QtGui -I/Library/Frameworks/QtOpenGL.framework/Versions/4/Headers -I/usr/include/QtOpenGL -I/usr/include -Iinclude -I/opt/local/include -I/home/i7709212/customlibs/glm -Iaccidentalnoise/include -I/usr/local/include -I/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -I/System/Library/Frameworks/AGL.framework/Headers -Imoc -Iui -I/usr/local/include -I/System/Library/Frameworks/CarbonCore.framework/Headers -F/Library/Frameworks
DEL_FILE  = rm -f
MOVE      = mv -f

IMAGES = 
PARSERS =
preprocess: $(PARSERS) compilers
clean preprocess_clean: parser_clean compiler_clean

parser_clean:
check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compilers: moc/moc_mainwindow.cpp moc/moc_OpenGLWidget.cpp ui/ui_mainwindow.h
compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_moc_header_make_all: moc/moc_mainwindow.cpp moc/moc_OpenGLWidget.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc/moc_mainwindow.cpp moc/moc_OpenGLWidget.cpp
moc/moc_mainwindow.cpp: include/OpenGLWidget.h \
		include/Camera.h \
		include/ShaderProgram.h \
		include/Shader.h \
		include/Texture.h \
		include/marchingcubes.h \
		include/terraingen.h \
		include/Trees.h \
		include/Model.h \
		include/Grass.h \
		include/grasshair.h \
		include/GeometryClipmap.h \
		include/noiseutils.h \
		include/Water.h \
		include/mainwindow.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ include/mainwindow.h -o moc/moc_mainwindow.cpp

moc/moc_OpenGLWidget.cpp: include/Camera.h \
		include/ShaderProgram.h \
		include/Shader.h \
		include/Texture.h \
		include/marchingcubes.h \
		include/terraingen.h \
		include/Trees.h \
		include/Model.h \
		include/Grass.h \
		include/grasshair.h \
		include/GeometryClipmap.h \
		include/noiseutils.h \
		include/Water.h \
		include/OpenGLWidget.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ include/OpenGLWidget.h -o moc/moc_OpenGLWidget.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_uic_make_all: ui/ui_mainwindow.h
compiler_uic_clean:
	-$(DEL_FILE) ui/ui_mainwindow.h
ui/ui_mainwindow.h: ui/mainwindow.ui
	/Developer/Tools/Qt/uic ui/mainwindow.ui -o ui/ui_mainwindow.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

