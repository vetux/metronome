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
    MainWindow();

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

    void selectSampleButtonPressed();

private:
    Metronome metronome;
    QWidget *centralWidget;
    QPushButton *controlButton;
    QSpinBox *bpmSpinBox;
    QLabel *sampleLabel;
    QPushButton *selectSampleButton;
};

#endif //METRONOME_MAINWINDOW_HPP
