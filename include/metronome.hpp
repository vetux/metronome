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

#ifndef METRONOME_METRONOME_HPP
#define METRONOME_METRONOME_HPP

#include <thread>
#include <functional>

#include "beatgenerator.hpp"
#include "sampleplayer.hpp"

class Metronome : BeatGenerator::Listener {
public:
    Metronome() {
        beatGenerator.addListener(this);
        //thread = std::thread(std::bind(&Metronome::loop, this, std::placeholders::_1));
        thread = std::thread([this]() { loop(); });
    }

    Metronome(int bpm, const std::string &samplePath) {
        beatGenerator.addListener(this);
        beatGenerator.setBPM(bpm);
        samplePlayer.setSamplePath(samplePath);
        thread = std::thread([this]() { loop(); });
    }

    ~Metronome() {
        runFlag = false;
        thread.join();
    }

    void setSamplePath(const std::string &path) {
        std::lock_guard<std::mutex> guard(mutex);
        samplePlayer.setSamplePath(path);
        beatGenerator.reset();
    }

    void setBPM(int bpm) {
        std::lock_guard<std::mutex> guard(mutex);
        beatGenerator.setBPM(bpm);
        beatGenerator.reset();
    }

    void start() {
        std::lock_guard<std::mutex> guard(mutex);
        beatGenerator.reset();
        playing = true;
    }

    void stop() {
        std::lock_guard<std::mutex> guard(mutex);
        playing = false;
    }

    bool isPlaying() {
        return playing;
    }

    void onBeat() override {
        samplePlayer.play();
    }

private:
    void loop() {
        while (runFlag) {
            std::lock_guard<std::mutex> guard(mutex);
            if (playing)
                beatGenerator.update();
            //TODO: Use precise sleep instead of full polling
        }
    }

    std::mutex mutex;

    bool runFlag = true;
    std::thread thread;

    bool playing = false;
    BeatGenerator beatGenerator;
    SamplePlayer samplePlayer;
};

#endif //METRONOME_METRONOME_HPP
