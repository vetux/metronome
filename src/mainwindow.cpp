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

#include "mainwindow.hpp"

#include "defaultsample.hpp"

MainWindow::MainWindow()
        : metronome() {
    const int defaultBPM = 40;

    metronome.setBPM(defaultBPM);
    metronome.setSampleData(std::string((char *) default_wav, default_wav_len));

    centralWidget = new QWidget();
    centralWidget->setLayout(new QVBoxLayout());
    setCentralWidget(centralWidget);

    controlButton = new QPushButton(this);
    controlButton->setText("Start");

    bpmSpinBox = new QSpinBox(this);
    bpmSpinBox->setMinimum(1);
    bpmSpinBox->setMaximum(std::numeric_limits<int>::max());
    bpmSpinBox->setValue(defaultBPM);

    auto sampleWidget = new QWidget(this);
    sampleWidget->setLayout(new QHBoxLayout());

    sampleLabel = new QLabel(this);
    sampleLabel->setText("Default Sample");

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

void MainWindow::selectSampleButtonPressed() {
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
                                  QString((std::string("Error: ") + e.what()).c_str()));
        }
        if (success) {
            sampleLabel->setText(path);
        }
    } else {
        if (QMessageBox::question(this, "Use Default Sample", "Do you want to use the default sample?")) {
            sampleLabel->setText("Default Sample");
            metronome.setSampleData(std::string((char *) default_wav, default_wav_len));
        }
    }
}
