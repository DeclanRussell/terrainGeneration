#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "OpenGLWidget.h"
#include <QProgressBar>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QProgressBar *m_progressBar;

private slots:
    void on_s_lowLandOctSld_sliderMoved(int position);

    void on_s_lowLandNoiseFreqSld_sliderMoved(int position);

    void on_s_lowLandOctSpn_valueChanged(int arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_s_highlandNoiseFreqSpn_valueChanged(double arg1);

    void on_s_highLandNoiseFreqSld_sliderMoved(int position);

    void on_s_genTerrainBtn_clicked();

    void on_spinBox_2_valueChanged(int arg1);

    void on_s_mountainNoiseFreqSpn_valueChanged(double arg1);

    void on_s_mountainNoiseSld_sliderMoved(int position);

    void on_s_genPreviewBtn_clicked();

    void on_s_caveAppBiasSld_sliderMoved(int position);

    void on_s_caveAppBiasSpn_valueChanged(double arg1);

    void on_s_caveThreshSld_sliderMoved(int position);

    void on_s_caveThreshSpn_valueChanged(double arg1);

    void on_s_marchResSpn_valueChanged(int arg1);

    void on_s_marchingCubesResSdl_sliderMoved(int position);

    void on_horizontalSlider_2_sliderMoved(int position);

    void on_horizontalSlider_sliderMoved(int position);

    void on_s_grassDenceSld_sliderMoved(int position);

    void on_s_grassDenseSpn_valueChanged(int arg1);

    void on_s_marchingCubesResSdl_sliderReleased();

private:
    Ui::MainWindow *ui;
    OpenGLWidget *m_openGLWidget;





};

#endif // MAINWINDOW_H
