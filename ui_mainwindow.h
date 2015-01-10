/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QGroupBox *s_highLandOctLbl;
    QGridLayout *gridLayout_2;
    QSpinBox *spinBox;
    QLabel *s_terrainPreviewLbl;
    QSlider *horizontalSlider;
    QSlider *s_MacGrasDenSld;
    QSlider *s_mountainNoiseSld;
    QDoubleSpinBox *s_mountainNoiseFreqSpn;
    QSlider *horizontalSlider_2;
    QPushButton *s_genPreviewBtn;
    QSpinBox *spinBox_2;
    QSlider *s_grassDenceSld;
    QSpinBox *s_marchResSpn;
    QSlider *s_caveThreshSld;
    QSpinBox *s_macGrasDenSpn;
    QLabel *s_marchResLbl;
    QLabel *s_grassDenseLbl;
    QSlider *s_lowLandNoiseFreqSld;
    QDoubleSpinBox *s_highlandNoiseFreqSpn;
    QSpacerItem *verticalSpacer;
    QLabel *s_marcoGrassDenLbl;
    QLabel *s_highLandNoiseFreqLbl;
    QLabel *s_lowLandNoiseFreqLbl;
    QLabel *s_mountainNoiseOctLbl;
    QDoubleSpinBox *s_caveThreshSpn;
    QSlider *s_marchingCubesResSdl;
    QSpinBox *s_lowLandOctSpn;
    QSpinBox *s_grassDenseSpn;
    QSlider *s_caveAppBiasSld;
    QLabel *s_caveAppBiasLbl;
    QDoubleSpinBox *s_caveAppBiasSpn;
    QLabel *s_caveThresholdLbl;
    QCheckBox *checkBox;
    QSlider *s_highLandNoiseFreqSld;
    QDoubleSpinBox *s_lowLandNoiseFreqSpn;
    QLabel *s_lowLandOctLbl;
    QPushButton *s_genTerrainBtn;
    QLabel *s_moutainNoiseFreqLbl;
    QLabel *label;
    QSlider *s_lowLandOctSld;
    QCheckBox *checkBox_2;
    QSpacerItem *horizontalSpacer_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1201, 917);
        MainWindow->setMinimumSize(QSize(500, 500));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 0, 1, 1);

        s_highLandOctLbl = new QGroupBox(centralwidget);
        s_highLandOctLbl->setObjectName(QStringLiteral("s_highLandOctLbl"));
        gridLayout_2 = new QGridLayout(s_highLandOctLbl);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        spinBox = new QSpinBox(s_highLandOctLbl);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setMaximum(8);
        spinBox->setValue(4);

        gridLayout_2->addWidget(spinBox, 5, 2, 1, 1);

        s_terrainPreviewLbl = new QLabel(s_highLandOctLbl);
        s_terrainPreviewLbl->setObjectName(QStringLiteral("s_terrainPreviewLbl"));
        s_terrainPreviewLbl->setMinimumSize(QSize(128, 128));

        gridLayout_2->addWidget(s_terrainPreviewLbl, 24, 1, 1, 1);

        horizontalSlider = new QSlider(s_highLandOctLbl);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setMaximum(8);
        horizontalSlider->setSingleStep(1);
        horizontalSlider->setValue(4);
        horizontalSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(horizontalSlider, 5, 1, 1, 1);

        s_MacGrasDenSld = new QSlider(s_highLandOctLbl);
        s_MacGrasDenSld->setObjectName(QStringLiteral("s_MacGrasDenSld"));
        s_MacGrasDenSld->setMaximum(3);
        s_MacGrasDenSld->setValue(2);
        s_MacGrasDenSld->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(s_MacGrasDenSld, 21, 1, 1, 1);

        s_mountainNoiseSld = new QSlider(s_highLandOctLbl);
        s_mountainNoiseSld->setObjectName(QStringLiteral("s_mountainNoiseSld"));
        s_mountainNoiseSld->setMaximum(300);
        s_mountainNoiseSld->setValue(100);
        s_mountainNoiseSld->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(s_mountainNoiseSld, 11, 1, 1, 1);

        s_mountainNoiseFreqSpn = new QDoubleSpinBox(s_highLandOctLbl);
        s_mountainNoiseFreqSpn->setObjectName(QStringLiteral("s_mountainNoiseFreqSpn"));
        s_mountainNoiseFreqSpn->setMaximum(3);
        s_mountainNoiseFreqSpn->setSingleStep(0.01);
        s_mountainNoiseFreqSpn->setValue(1);

        gridLayout_2->addWidget(s_mountainNoiseFreqSpn, 11, 2, 1, 1);

        horizontalSlider_2 = new QSlider(s_highLandOctLbl);
        horizontalSlider_2->setObjectName(QStringLiteral("horizontalSlider_2"));
        horizontalSlider_2->setMaximum(8);
        horizontalSlider_2->setValue(8);
        horizontalSlider_2->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(horizontalSlider_2, 9, 1, 1, 1);

        s_genPreviewBtn = new QPushButton(s_highLandOctLbl);
        s_genPreviewBtn->setObjectName(QStringLiteral("s_genPreviewBtn"));

        gridLayout_2->addWidget(s_genPreviewBtn, 25, 1, 1, 1);

        spinBox_2 = new QSpinBox(s_highLandOctLbl);
        spinBox_2->setObjectName(QStringLiteral("spinBox_2"));
        spinBox_2->setMaximum(8);
        spinBox_2->setValue(8);

        gridLayout_2->addWidget(spinBox_2, 9, 2, 1, 1);

        s_grassDenceSld = new QSlider(s_highLandOctLbl);
        s_grassDenceSld->setObjectName(QStringLiteral("s_grassDenceSld"));
        s_grassDenceSld->setMaximum(3);
        s_grassDenceSld->setValue(3);
        s_grassDenceSld->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(s_grassDenceSld, 19, 1, 1, 1);

        s_marchResSpn = new QSpinBox(s_highLandOctLbl);
        s_marchResSpn->setObjectName(QStringLiteral("s_marchResSpn"));
        s_marchResSpn->setMaximum(128);
        s_marchResSpn->setValue(32);

        gridLayout_2->addWidget(s_marchResSpn, 17, 2, 1, 1);

        s_caveThreshSld = new QSlider(s_highLandOctLbl);
        s_caveThreshSld->setObjectName(QStringLiteral("s_caveThreshSld"));
        s_caveThreshSld->setMaximum(100);
        s_caveThreshSld->setValue(48);
        s_caveThreshSld->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(s_caveThreshSld, 15, 1, 1, 1);

        s_macGrasDenSpn = new QSpinBox(s_highLandOctLbl);
        s_macGrasDenSpn->setObjectName(QStringLiteral("s_macGrasDenSpn"));
        s_macGrasDenSpn->setMaximum(3);
        s_macGrasDenSpn->setValue(2);

        gridLayout_2->addWidget(s_macGrasDenSpn, 21, 2, 1, 1);

        s_marchResLbl = new QLabel(s_highLandOctLbl);
        s_marchResLbl->setObjectName(QStringLiteral("s_marchResLbl"));

        gridLayout_2->addWidget(s_marchResLbl, 16, 1, 1, 1);

        s_grassDenseLbl = new QLabel(s_highLandOctLbl);
        s_grassDenseLbl->setObjectName(QStringLiteral("s_grassDenseLbl"));

        gridLayout_2->addWidget(s_grassDenseLbl, 18, 1, 1, 1);

        s_lowLandNoiseFreqSld = new QSlider(s_highLandOctLbl);
        s_lowLandNoiseFreqSld->setObjectName(QStringLiteral("s_lowLandNoiseFreqSld"));
        s_lowLandNoiseFreqSld->setMaximum(300);
        s_lowLandNoiseFreqSld->setValue(25);
        s_lowLandNoiseFreqSld->setSliderPosition(25);
        s_lowLandNoiseFreqSld->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(s_lowLandNoiseFreqSld, 3, 1, 1, 1);

        s_highlandNoiseFreqSpn = new QDoubleSpinBox(s_highLandOctLbl);
        s_highlandNoiseFreqSpn->setObjectName(QStringLiteral("s_highlandNoiseFreqSpn"));
        s_highlandNoiseFreqSpn->setMaximum(3);
        s_highlandNoiseFreqSpn->setSingleStep(0.01);
        s_highlandNoiseFreqSpn->setValue(2);

        gridLayout_2->addWidget(s_highlandNoiseFreqSpn, 7, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 27, 1, 1, 1);

        s_marcoGrassDenLbl = new QLabel(s_highLandOctLbl);
        s_marcoGrassDenLbl->setObjectName(QStringLiteral("s_marcoGrassDenLbl"));

        gridLayout_2->addWidget(s_marcoGrassDenLbl, 20, 1, 1, 1);

        s_highLandNoiseFreqLbl = new QLabel(s_highLandOctLbl);
        s_highLandNoiseFreqLbl->setObjectName(QStringLiteral("s_highLandNoiseFreqLbl"));

        gridLayout_2->addWidget(s_highLandNoiseFreqLbl, 6, 1, 1, 1);

        s_lowLandNoiseFreqLbl = new QLabel(s_highLandOctLbl);
        s_lowLandNoiseFreqLbl->setObjectName(QStringLiteral("s_lowLandNoiseFreqLbl"));

        gridLayout_2->addWidget(s_lowLandNoiseFreqLbl, 2, 1, 1, 1);

        s_mountainNoiseOctLbl = new QLabel(s_highLandOctLbl);
        s_mountainNoiseOctLbl->setObjectName(QStringLiteral("s_mountainNoiseOctLbl"));

        gridLayout_2->addWidget(s_mountainNoiseOctLbl, 8, 1, 1, 1);

        s_caveThreshSpn = new QDoubleSpinBox(s_highLandOctLbl);
        s_caveThreshSpn->setObjectName(QStringLiteral("s_caveThreshSpn"));
        s_caveThreshSpn->setMaximum(1);
        s_caveThreshSpn->setSingleStep(0.01);
        s_caveThreshSpn->setValue(0.48);

        gridLayout_2->addWidget(s_caveThreshSpn, 15, 2, 1, 1);

        s_marchingCubesResSdl = new QSlider(s_highLandOctLbl);
        s_marchingCubesResSdl->setObjectName(QStringLiteral("s_marchingCubesResSdl"));
        s_marchingCubesResSdl->setMaximum(128);
        s_marchingCubesResSdl->setValue(32);
        s_marchingCubesResSdl->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(s_marchingCubesResSdl, 17, 1, 1, 1);

        s_lowLandOctSpn = new QSpinBox(s_highLandOctLbl);
        s_lowLandOctSpn->setObjectName(QStringLiteral("s_lowLandOctSpn"));
        s_lowLandOctSpn->setMaximum(8);
        s_lowLandOctSpn->setValue(2);

        gridLayout_2->addWidget(s_lowLandOctSpn, 1, 2, 1, 1);

        s_grassDenseSpn = new QSpinBox(s_highLandOctLbl);
        s_grassDenseSpn->setObjectName(QStringLiteral("s_grassDenseSpn"));
        s_grassDenseSpn->setMaximum(3);
        s_grassDenseSpn->setValue(3);

        gridLayout_2->addWidget(s_grassDenseSpn, 19, 2, 1, 1);

        s_caveAppBiasSld = new QSlider(s_highLandOctLbl);
        s_caveAppBiasSld->setObjectName(QStringLiteral("s_caveAppBiasSld"));
        s_caveAppBiasSld->setMaximum(100);
        s_caveAppBiasSld->setValue(30);
        s_caveAppBiasSld->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(s_caveAppBiasSld, 13, 1, 1, 1);

        s_caveAppBiasLbl = new QLabel(s_highLandOctLbl);
        s_caveAppBiasLbl->setObjectName(QStringLiteral("s_caveAppBiasLbl"));

        gridLayout_2->addWidget(s_caveAppBiasLbl, 12, 1, 1, 1);

        s_caveAppBiasSpn = new QDoubleSpinBox(s_highLandOctLbl);
        s_caveAppBiasSpn->setObjectName(QStringLiteral("s_caveAppBiasSpn"));
        s_caveAppBiasSpn->setMaximum(1);
        s_caveAppBiasSpn->setSingleStep(0.01);
        s_caveAppBiasSpn->setValue(0.3);

        gridLayout_2->addWidget(s_caveAppBiasSpn, 13, 2, 1, 1);

        s_caveThresholdLbl = new QLabel(s_highLandOctLbl);
        s_caveThresholdLbl->setObjectName(QStringLiteral("s_caveThresholdLbl"));

        gridLayout_2->addWidget(s_caveThresholdLbl, 14, 1, 1, 1);

        checkBox = new QCheckBox(s_highLandOctLbl);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        gridLayout_2->addWidget(checkBox, 22, 1, 1, 1);

        s_highLandNoiseFreqSld = new QSlider(s_highLandOctLbl);
        s_highLandNoiseFreqSld->setObjectName(QStringLiteral("s_highLandNoiseFreqSld"));
        s_highLandNoiseFreqSld->setMaximum(300);
        s_highLandNoiseFreqSld->setValue(200);
        s_highLandNoiseFreqSld->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(s_highLandNoiseFreqSld, 7, 1, 1, 1);

        s_lowLandNoiseFreqSpn = new QDoubleSpinBox(s_highLandOctLbl);
        s_lowLandNoiseFreqSpn->setObjectName(QStringLiteral("s_lowLandNoiseFreqSpn"));
        s_lowLandNoiseFreqSpn->setMaximum(3);
        s_lowLandNoiseFreqSpn->setSingleStep(0.01);
        s_lowLandNoiseFreqSpn->setValue(0.25);

        gridLayout_2->addWidget(s_lowLandNoiseFreqSpn, 3, 2, 1, 1);

        s_lowLandOctLbl = new QLabel(s_highLandOctLbl);
        s_lowLandOctLbl->setObjectName(QStringLiteral("s_lowLandOctLbl"));

        gridLayout_2->addWidget(s_lowLandOctLbl, 0, 1, 1, 1);

        s_genTerrainBtn = new QPushButton(s_highLandOctLbl);
        s_genTerrainBtn->setObjectName(QStringLiteral("s_genTerrainBtn"));

        gridLayout_2->addWidget(s_genTerrainBtn, 26, 1, 1, 1);

        s_moutainNoiseFreqLbl = new QLabel(s_highLandOctLbl);
        s_moutainNoiseFreqLbl->setObjectName(QStringLiteral("s_moutainNoiseFreqLbl"));

        gridLayout_2->addWidget(s_moutainNoiseFreqLbl, 10, 1, 1, 1);

        label = new QLabel(s_highLandOctLbl);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 4, 1, 1, 1);

        s_lowLandOctSld = new QSlider(s_highLandOctLbl);
        s_lowLandOctSld->setObjectName(QStringLiteral("s_lowLandOctSld"));
        s_lowLandOctSld->setMaximum(8);
        s_lowLandOctSld->setValue(2);
        s_lowLandOctSld->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(s_lowLandOctSld, 1, 1, 1, 1);

        checkBox_2 = new QCheckBox(s_highLandOctLbl);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        gridLayout_2->addWidget(checkBox_2, 23, 1, 1, 1);


        gridLayout->addWidget(s_highLandOctLbl, 0, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1201, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);
        QObject::connect(spinBox, SIGNAL(valueChanged(int)), horizontalSlider, SLOT(setValue(int)));
        QObject::connect(horizontalSlider, SIGNAL(sliderMoved(int)), spinBox, SLOT(setValue(int)));
        QObject::connect(spinBox_2, SIGNAL(valueChanged(int)), horizontalSlider_2, SLOT(setValue(int)));
        QObject::connect(horizontalSlider_2, SIGNAL(sliderMoved(int)), spinBox_2, SLOT(setValue(int)));
        QObject::connect(s_marchingCubesResSdl, SIGNAL(sliderMoved(int)), s_marchResSpn, SLOT(setValue(int)));
        QObject::connect(s_marchResSpn, SIGNAL(valueChanged(int)), s_marchingCubesResSdl, SLOT(setValue(int)));
        QObject::connect(s_grassDenseSpn, SIGNAL(valueChanged(int)), s_grassDenceSld, SLOT(setValue(int)));
        QObject::connect(s_grassDenceSld, SIGNAL(sliderMoved(int)), s_grassDenseSpn, SLOT(setValue(int)));
        QObject::connect(s_MacGrasDenSld, SIGNAL(sliderMoved(int)), s_macGrasDenSpn, SLOT(setValue(int)));
        QObject::connect(s_macGrasDenSpn, SIGNAL(valueChanged(int)), s_MacGrasDenSld, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        s_highLandOctLbl->setTitle(QApplication::translate("MainWindow", "Terrain Properties", 0));
        s_terrainPreviewLbl->setText(QString());
        s_genPreviewBtn->setText(QApplication::translate("MainWindow", "Generate Preview", 0));
        s_marchResLbl->setText(QApplication::translate("MainWindow", "Marching Cubes Resolution", 0));
        s_grassDenseLbl->setText(QApplication::translate("MainWindow", "Meso Grass Density", 0));
        s_marcoGrassDenLbl->setText(QApplication::translate("MainWindow", "Macro Grass Density", 0));
        s_highLandNoiseFreqLbl->setText(QApplication::translate("MainWindow", "High Land Noise Frequency", 0));
        s_lowLandNoiseFreqLbl->setText(QApplication::translate("MainWindow", "Low Land Noise Frequency", 0));
        s_mountainNoiseOctLbl->setText(QApplication::translate("MainWindow", "Mountain Noise Octaves", 0));
        s_caveAppBiasLbl->setText(QApplication::translate("MainWindow", "Cave Attenuate Bias", 0));
        s_caveThresholdLbl->setText(QApplication::translate("MainWindow", "Cave Threshhold", 0));
        checkBox->setText(QApplication::translate("MainWindow", "Wireframe", 0));
        s_lowLandOctLbl->setText(QApplication::translate("MainWindow", "Low Land Noise Octaves", 0));
        s_genTerrainBtn->setText(QApplication::translate("MainWindow", "Generate Terrain", 0));
        s_moutainNoiseFreqLbl->setText(QApplication::translate("MainWindow", "Mountain Noise Frequency", 0));
        label->setText(QApplication::translate("MainWindow", "High Land Noise Octaves", 0));
        checkBox_2->setText(QApplication::translate("MainWindow", "Grass", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
