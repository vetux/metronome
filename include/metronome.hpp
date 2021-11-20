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
#include <condition_variable>

#include "beatgenerator.hpp"
#include "sampleplayer.hpp"

class Metronome {
public:
    /**
     * @param beatInterval How many times per second should the metronome thread update the beat generator, Corresponds to the maximum bpm
     */
    Metronome(int beatInterval = 20)
            : beatInterval(beatInterval) {
        //thread = std::thread(std::bind(&Metronome::loop, this, std::placeholders::_1));
        thread = std::thread([this]() { loop(); });
    }

    Metronome(int beatInterval, int bpm, const std::string &samplePath)
            : beatInterval(beatInterval) {
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

    void setSampleData(const std::string &data) {
        std::lock_guard<std::mutex> guard(mutex);
        samplePlayer.setSampleData(data);
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
        playingCondition.notify_all();
    }

    void stop() {
        std::lock_guard<std::mutex> guard(mutex);
        playing = false;
        samplePlayer.stop();
    }

    bool isPlaying() {
        return playing;
    }

private:
    void loop() {
        while (runFlag) {
            std::unique_lock<std::mutex> guard(mutex);
            if (playing) {
                //TODO: Fix drift
                // The beat generator update includes the time any cycles between update invocations and
                // the beat generator instructions themselves took so there should not be any drift caused by the beat generator.
                // Most likely a problem with OpenALSoft latency as it spins another thread.
                auto time = beatGenerator.update();
                if (time.count() == 0) {
                    samplePlayer.play();
                } else {
                    guard.unlock();
                    std::this_thread::sleep_for(std::chrono::duration<double>(
                            1.0f / static_cast<double>(beatInterval)));
                }
            } else {
                playingCondition.wait(guard, [this] {
                    if (playing)
                        return true;
                    else
                        return false;
                });
            }
        }
    }

    std::mutex mutex;

    bool runFlag = true;
    std::thread thread;

    int beatInterval;

    bool playing = false;
    std::condition_variable playingCondition;

    BeatGenerator beatGenerator;
    SamplePlayer samplePlayer;
};

#endif //METRONOME_METRONOME_HPP
