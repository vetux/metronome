/**
 *  Metronome - A Desktop Metronome application
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef METRONOME_MAINWINDOW_HPP
#define METRONOME_MAINWINDOW_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <thread>

#include "metronome.hpp"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    MainWindow()
            : metronome(40, "samples/default.wav") {
        centralWidget = new QWidget();
        centralWidget->setLayout(new QVBoxLayout());
        setCentralWidget(centralWidget);

        controlButton = new QPushButton(this);
        controlButton->setText("Start");

        bpmSpinBox = new QSpinBox(this);
        bpmSpinBox->setMinimum(1);
        bpmSpinBox->setMaximum(std::numeric_limits<int>::max());
        bpmSpinBox->setValue(40);

        auto sampleWidget = new QWidget(this);
        sampleWidget->setLayout(new QHBoxLayout());

        sampleLabel = new QLabel(this);
        sampleLabel->setText("samples/default.wav");

        selectSampleButton = new QPushButton(this);
        selectSampleButton->setText("Select Sample");

        sampleWidget->layout()->addWidget(sampleLabel);
        sampleWidget->layout()->addWidget(selectSampleButton);

        connect(controlButton, SIGNAL(pressed()), this, SLOT(toggle()));
        connect(bpmSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setBPM(int)));
        connect(selectSampleButton, SIGNAL(pressed()), this, SLOT(selectSampleButtonPressed()));

        centralWidget->layout()->addWidget(controlButton);
        centralWidget->layout()->addWidget(bpmSpinBox);
        centralWidget->layout()->addWidget(sampleWidget);
        centralWidget->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    }

public slots:

    void toggle() {
        if (metronome.isPlaying())
            stop();
        else
            start();
    };

    void start() {
        metronome.start();
        controlButton->setText("Stop");
    }

    void stop() {
        metronome.stop();
        controlButton->setText("Start");
    }

    void setSamplePath(const std::string &filePath) {
        metronome.setSamplePath(filePath);
    }

    void setBPM(int bpm) {
        metronome.setBPM(bpm);
    }

    void selectSampleButtonPressed() {
        stop();
        auto path = QFileDialog::getOpenFileName(this, tr("Select Audio Sample"));
        if (!path.isNull()) {
            bool success = false;
            try {
                setSamplePath(path.toStdString());
                success = true;
            } catch (std::exception &e) {
                QMessageBox::critical(this,
                                      QString("Failed to open Audio Sample"),
                                      QString(("Failed to open audio sample at " + path.toStdString() + ", Error: " +
                                               e.what()).c_str()));
            }
            if (success) {
                sampleLabel->setText(path);
            }
        }
    }

private:
    Metronome metronome;
    QWidget *centralWidget;
    QPushButton *controlButton;
    QSpinBox *bpmSpinBox;
    QLabel *sampleLabel;
    QPushButton *selectSampleButton;
};

#endif //METRONOME_MAINWINDOW_HPP
