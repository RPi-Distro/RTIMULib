////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib
//
//  Copyright (c) 2014-2015, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "MagCalDlg.h"
#include "RTIMUMagCal.h"

#include <qboxlayout.h>
#include <qgridlayout.h>
#include <qformlayout.h>
#include <qprocess.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <qfileinfo.h>

MagCalDlg::MagCalDlg(QWidget *parent, RTIMUSettings* settings)
    : QDialog(parent)
{
    m_cal = new RTIMUMagCal(settings);
    m_newData = false;

    m_fitDirOptions.append(RTIMUCALDEFS_OCTAVE_PATH);

    findFitDir();

    m_cal->magCalInit();

    m_minMaxMode = true;
    layoutWindow();
    setButtonEnables();

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_timer = startTimer(50);

    connect(m_cancelBtn, SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(m_resetBtn, SIGNAL(clicked()), this, SLOT(onReset()));
    connect(m_saveMinMaxBtn, SIGNAL(clicked()), this, SLOT(onSaveMinMax()));
    connect(m_processEllipsoidBtn, SIGNAL(clicked()), this, SLOT(onProcess()));
}

MagCalDlg::~MagCalDlg()
{
}

void MagCalDlg::newIMUData(const RTIMU_DATA& data)
{
    QMutexLocker lock(&m_refreshMutex);

    m_currentVal = data.compass;

    if (m_minMaxMode)
        m_cal->newMinMaxData(data.compass);
    else
        m_cal->newEllipsoidData(data.compass);

    m_newData = true;
}

void MagCalDlg::findFitDir()
{
    m_fitDir = "";
    m_usingEllipsoidFit = false;

    for (int i = 0; i < m_fitDirOptions.count(); i++) {
        QFileInfo file(m_fitDirOptions.at(i) + RTIMUCALDEFS_OCTAVE_CODE);

        if (file.exists()) {
            m_fitDir = m_fitDirOptions.at(i);
            m_usingEllipsoidFit = true;
            return;
        }
    }
}

void MagCalDlg::onCancel()
{
    killTimer(m_timer);
    reject();
}

void MagCalDlg::onReset()
{
    m_cal->magCalReset();
    m_minMaxMode = true;
    setButtonEnables();
}

void MagCalDlg::onSaveMinMax()
{
    m_cal->magCalSaveMinMax();

    if (m_usingEllipsoidFit) {
        m_minMaxMode = false;
        setButtonEnables();
    } else {
        accept();
    }
}

void MagCalDlg::onProcess()
{
    m_cal->magCalSaveRaw(".");

    QProcess proc;

    proc.start(RTIMUCALDEFS_OCTAVE_COMMAND);
    proc.waitForFinished(20000);
    if (proc.exitCode() == 0) {
        m_cal->magCalSaveCorr(".");
    } else {
        QMessageBox::warning(this, "Ellipsoid fit error",
            "Failed to execute RTEllipsoidFit.m. Only min/max calibration available",
            QMessageBox::Ok);
    }
    accept();
}

void MagCalDlg::timerEvent(QTimerEvent *)
{
    QMutexLocker lock(&m_refreshMutex);

    if (m_newData)
        updateControls();
    m_newData = false;
}

void MagCalDlg::updateControls()
{
    for (int i = 0; i < 3; i++) {
        setRaw(m_raw[i], m_currentVal.data(i));
        setRawMinMax(m_rawMin[i], m_cal->m_magMin.data(i));
        setRawMinMax(m_rawMax[i], m_cal->m_magMax.data(i));
    }
    if (m_usingEllipsoidFit)
        setOctantCounts();

    if (m_minMaxMode) {
        if (!m_saveMinMaxBtn->isEnabled() && m_cal->magCalValid())
            m_saveMinMaxBtn->setEnabled(true);
    } else {
        if (!m_processEllipsoidBtn->isEnabled() && m_cal->magCalEllipsoidValid())
            m_processEllipsoidBtn->setEnabled(true);
    }
}

void MagCalDlg::setRaw(QLabel *label, float val)
{
    label->setText(QString::number(val, 'f', 4));

    if (val < 0)
        label->setStyleSheet(m_redStyleSheet);
    else
        label->setStyleSheet(m_greenStyleSheet);
}

void MagCalDlg::setRawMinMax(QLabel *label, float val)
{
    label->setText(QString::number(val, 'f', 4));

    if (val < 0)
        label->setStyleSheet(m_lightRedStyleSheet);
    else
        label->setStyleSheet(m_lightGreenStyleSheet);
}

void MagCalDlg::setOctantCounts()
{
    int counts[RTIMUCALDEFS_OCTANT_COUNT];

    m_cal->magCalOctantCounts(counts);

    for (int i = 0; i < RTIMUCALDEFS_OCTANT_COUNT; i++) {
        m_octantCount[i]->setText(QString::number(counts[i]));
    }
}

void MagCalDlg::layoutWindow()
{
    QHBoxLayout *hLayout;
    QLabel *label;
    QSpacerItem *spacer;
    int gridRow = 0;

    QString octantNames[] = {"---", "+--", "-+-", "++-", "--+", "+-+", "-++", "+++"};

    m_whiteStyleSheet = QString::fromUtf8("background-color: rgb(255, 255, 255);");
    m_lightRedStyleSheet = QString::fromUtf8("background-color: rgb(200, 100, 100);");
    m_lightGreenStyleSheet = QString::fromUtf8("background-color: rgb(100, 200, 100);");
    m_redStyleSheet = QString::fromUtf8("background-color: rgb(220, 80, 80);");
    m_greenStyleSheet = QString::fromUtf8("background-color: rgb(38, 244, 54);");

    QVBoxLayout *centralLayout = new QVBoxLayout(this);
    centralLayout->setSpacing(20);
    centralLayout->setContentsMargins(6, 6, 6, 6);

    QGridLayout *gridLayout = new QGridLayout();

    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(4, 4, 4, 4);

    hLayout = new QHBoxLayout();
    hLayout->setSpacing(4);

    spacer = new QSpacerItem(80, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
    hLayout->addItem(spacer);

    label = getFixedLabel("XMin", 80, 20, Qt::AlignCenter);
    hLayout->addWidget(label);
    label = getFixedLabel("X", 80, 20, Qt::AlignCenter);
    hLayout->addWidget(label);
    label = getFixedLabel("XMax", 80, 20, Qt::AlignCenter);
    hLayout->addWidget(label);

    spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
    hLayout->addItem(spacer);

    label = getFixedLabel("YMin", 80, 20, Qt::AlignCenter);
    hLayout->addWidget(label);
    label = getFixedLabel("Y", 80, 20, Qt::AlignCenter);
    hLayout->addWidget(label);
    label = getFixedLabel("YMax", 80, 20, Qt::AlignCenter);
    hLayout->addWidget(label);

    spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
    hLayout->addItem(spacer);

    label = getFixedLabel("ZMin", 80, 20, Qt::AlignCenter);
    hLayout->addWidget(label);
    label = getFixedLabel("Z", 80, 20, Qt::AlignCenter);
    hLayout->addWidget(label);
    label = getFixedLabel("ZMax", 80, 20, Qt::AlignCenter);
    hLayout->addWidget(label);

    gridLayout->addLayout(hLayout, gridRow++, 0, 1, 1);
    spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridLayout->addItem(spacer, gridRow++, 0, 1, 1);

    // the current row
    hLayout = new QHBoxLayout();
    hLayout->setSpacing(4);

    label = getFixedLabel("Current ", 80, 20, Qt::AlignLeft | Qt::AlignVCenter);
    hLayout->addWidget(label);

    for (int j = 0; j < 3; j++) {
        m_rawMin[j] = getFixedLabel("0", 80, 20, Qt::AlignCenter, m_whiteStyleSheet);
        hLayout->addWidget(m_rawMin[j]);
        m_raw[j] = getFixedLabel("0", 80, 20, Qt::AlignCenter, m_whiteStyleSheet);
        hLayout->addWidget(m_raw[j]);
        m_rawMax[j] = getFixedLabel("0", 80, 20, Qt::AlignCenter, m_whiteStyleSheet);
        hLayout->addWidget(m_rawMax[j]);

        if (j < 2) {
            spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
            hLayout->addItem(spacer);
        }
    }

    gridLayout->addLayout(hLayout, gridRow++, 0, 1, 1);
    spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridLayout->addItem(spacer, gridRow++, 0, 1, 1);

    centralLayout->addLayout(gridLayout);

    if (m_usingEllipsoidFit) {
        //  Do octant displays

        centralLayout->addWidget(new QLabel("Octant counts:"));
        QGridLayout *octantLayout = new QGridLayout();
        octantLayout->setSpacing(6);
        octantLayout->setContentsMargins(4, 4, 4, 4);

        for (int i = 0; i < RTIMUCALDEFS_OCTANT_COUNT; i++) {
            QHBoxLayout *hOctant = new QHBoxLayout();
            QLabel *label = getFixedLabel(octantNames[i], 50, 20, Qt::AlignCenter, m_whiteStyleSheet);
            m_octantCount[i] = getFixedLabel("0", 50, 20, Qt::AlignCenter, m_whiteStyleSheet);
            hOctant->addWidget(label);
            hOctant->addWidget(m_octantCount[i]);
            octantLayout->addLayout(hOctant, i / 4, i % 4);
        }
        centralLayout->addLayout(octantLayout);
    }

    QHBoxLayout *hBox = new QHBoxLayout();
    centralLayout->addLayout(hBox);
    QFormLayout *formLayout = new QFormLayout();
    hBox->addLayout(formLayout);
    hBox->setAlignment(Qt::AlignHCenter);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_processEllipsoidBtn = new QPushButton("Process ellipsoid");
    m_saveMinMaxBtn = new QPushButton("Save min/max");
    m_resetBtn = new QPushButton("Reset");
    m_cancelBtn = new QPushButton("Cancel");

    buttonLayout->addWidget(m_resetBtn);
    buttonLayout->addWidget(m_saveMinMaxBtn);
    if (m_usingEllipsoidFit)
        buttonLayout->addWidget(m_processEllipsoidBtn);
    buttonLayout->addWidget(m_cancelBtn);

    hBox->addLayout(buttonLayout);

    hLayout = new QHBoxLayout();
    hLayout->addSpacing(24);

    hLayout->addStretch();

    centralLayout->addLayout(hLayout);
}

QLabel* MagCalDlg::getFixedLabel(QString text, int w, int h, Qt::Alignment alignment, QString styleSheet)
{
    QLabel *label = new QLabel(text);

    label->setMaximumSize(QSize(w + 10, h));
    label->setMinimumSize(QSize(w, h));
    label->setAlignment(alignment);

    if (styleSheet.length() > 0)
        label->setStyleSheet(styleSheet);

    return label;
}

void MagCalDlg::setButtonEnables()
{
    m_saveMinMaxBtn->setEnabled(false);
    if (m_usingEllipsoidFit)
        m_processEllipsoidBtn->setEnabled(false);
    if (m_minMaxMode) {
        setWindowTitle("Magnetomer Calibration - collecting min/max data");
    } else {
        setWindowTitle(QString("Magnetomer Calibration - collecting ellipsoid data (need %1 in each octant)")
            .arg(RTIMUCALDEFS_OCTANT_MIN_SAMPLES));
    }
}
