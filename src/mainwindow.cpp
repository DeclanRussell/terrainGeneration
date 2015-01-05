#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImage>
#include <QPicture>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    QGLFormat format;
    format.setVersion(4,1);
    format.setProfile(QGLFormat::CoreProfile);

    m_openGLWidget = new OpenGLWidget(format,this);
    ui->gridLayout->addWidget(m_openGLWidget,0,0,1,2);
    m_progressBar = new QProgressBar(this);
    ui->gridLayout->addWidget(m_progressBar,1,1,1,1);
    m_progressBar->setVisible(false);


}

MainWindow::~MainWindow(){
    delete ui;
    delete m_openGLWidget;

}


void MainWindow::on_s_lowLandOctSld_sliderMoved(int position)
{
    ui->s_lowLandOctSpn->setValue(position);
    m_openGLWidget->m_terrainFactory->setLowLandOctaves(position);
}

void MainWindow::on_s_lowLandNoiseFreqSld_sliderMoved(int position)
{
    ui->s_lowLandNoiseFreqSpn->setValue(position/100.0);
    m_openGLWidget->m_terrainFactory->setLowLandFreq(position/100.0);
}

void MainWindow::on_s_lowLandOctSpn_valueChanged(int arg1)
{
    ui->s_lowLandOctSld->setValue(arg1);
    m_openGLWidget->m_terrainFactory->setLowLandOctaves(arg1);
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    m_openGLWidget->m_terrainFactory->setHighLandOctaves(arg1);
}

void MainWindow::on_s_highlandNoiseFreqSpn_valueChanged(double arg1)
{
    m_openGLWidget->m_terrainFactory->setHighLandFreq(arg1);
    ui->s_highLandNoiseFreqSld->setValue(arg1*100);
}

void MainWindow::on_s_highLandNoiseFreqSld_sliderMoved(int position)
{
    m_openGLWidget->m_terrainFactory->setHighLandFreq(position/100.0);
    ui->s_highlandNoiseFreqSpn->setValue(position/100.0);
}

void MainWindow::on_s_genTerrainBtn_clicked()
{
    m_openGLWidget->m_terrainFactory->setProgressBar(m_progressBar);
    m_openGLWidget->m_terrainFactory->createTerrainFromNoise();
    m_openGLWidget->m_marchingCubesObject->vMarchingCubes();
    m_openGLWidget->m_geometryClipmap->setHeightMap(m_openGLWidget->m_terrainFactory->createHeightMap(512,512));

}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    m_openGLWidget->m_terrainFactory->setMountainOctaves(arg1);
}

void MainWindow::on_s_mountainNoiseFreqSpn_valueChanged(double arg1)
{
    m_openGLWidget->m_terrainFactory->setMountainFreq(arg1);
    ui->s_mountainNoiseSld->setValue(arg1*100);
}

void MainWindow::on_s_mountainNoiseSld_sliderMoved(int position)
{
    m_openGLWidget->m_terrainFactory->setMountainFreq(position/100.0);
    ui->s_mountainNoiseFreqSpn->setValue(position/100.0);
}

void MainWindow::on_s_genPreviewBtn_clicked()
{
    m_openGLWidget->m_terrainFactory->setProgressBar(m_progressBar);
    QImage preview = m_openGLWidget->m_terrainFactory->create2DNoise();
    ui->s_terrainPreviewLbl->setPixmap(QPixmap::fromImage(preview));

}

void MainWindow::on_s_caveAppBiasSld_sliderMoved(int position)
{
    m_openGLWidget->m_terrainFactory->setCaveAttBias(position/100.0);
    ui->s_caveAppBiasSpn->setValue(position/100.0);
}

void MainWindow::on_s_caveAppBiasSpn_valueChanged(double arg1)
{
    m_openGLWidget->m_terrainFactory->setCaveAttBias(arg1);
    ui->s_caveAppBiasSld->setValue(arg1*100);
}

void MainWindow::on_s_caveThreshSld_sliderMoved(int position)
{
    m_openGLWidget->m_terrainFactory->setCaveThreshold(position/100.0);
    ui->s_caveThreshSpn->setValue(position/100.0);
}

void MainWindow::on_s_caveThreshSpn_valueChanged(double arg1)
{
    m_openGLWidget->m_terrainFactory->setCaveThreshold(arg1);
    ui->s_caveThreshSld->setValue(arg1*100);
}

void MainWindow::on_s_marchResSpn_valueChanged(int arg1)
{
    m_openGLWidget->m_marchingCubesObject->setSampleResolution(arg1);
    m_openGLWidget->m_marchingCubesObject->vMarchingCubes();
}

void MainWindow::on_s_marchingCubesResSdl_sliderMoved(int position)
{
    m_openGLWidget->m_marchingCubesObject->setSampleResolution(position);
}

void MainWindow::on_horizontalSlider_2_sliderMoved(int position)
{
    m_openGLWidget->m_terrainFactory->setMountainOctaves(position);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    m_openGLWidget->m_terrainFactory->setHighLandOctaves(position);
}

void MainWindow::on_s_grassDenceSld_sliderMoved(int position)
{
    m_openGLWidget->m_grassHairFactory->setNumStrandsPerFace(position);
}

void MainWindow::on_s_grassDenseSpn_valueChanged(int arg1)
{
    m_openGLWidget->m_grassHairFactory->setNumStrandsPerFace(arg1);
}

void MainWindow::on_s_marchingCubesResSdl_sliderReleased()
{
    m_openGLWidget->m_marchingCubesObject->setSampleResolution(ui->s_marchingCubesResSdl->value());
    m_openGLWidget->m_marchingCubesObject->vMarchingCubes();
}
