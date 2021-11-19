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

#ifndef METRONOME_SAMPLEPLAYER_HPP
#define METRONOME_SAMPLEPLAYER_HPP

#include <mutex>

#include "audio/audiodevice.hpp"

#include "audioloader.hpp"

class SamplePlayer {
public:
    /**
     * @param numberOfSources The number of audio sources to create for playing back the samples. This corresponds to the maximum concurrently playing samples.
     */
    SamplePlayer(int numberOfSources = 10) {
        audioDevice = engine::AudioDevice::createDevice(engine::OpenAL);
        audioContext = audioDevice->createContext();
        audioContext->makeCurrent();
        for (int i = 0; i < numberOfSources; i++) {
            audioSources.emplace_back(audioContext->createSource());
        }
    }

    explicit SamplePlayer(int numberOfSources, const std::string &samplePath) {
        audioDevice = engine::AudioDevice::createDevice(engine::OpenAL);
        audioContext = audioDevice->createContext();
        audioContext->makeCurrent();
        for (int i = 0; i < numberOfSources; i++) {
            audioSources.emplace_back(audioContext->createSource());
        }
        setSamplePath(samplePath);
    }

    void play() {
        if (audioSample == nullptr) {
            throw std::runtime_error("No sample loaded");
        }

        auto &source = audioSources.at(sourceIndex++);

        if (sourceIndex >= audioSources.size())
            sourceIndex = 0;

        source->stop();
        source->setBuffer(*audioSample);
        source->play();
    }

    void setSamplePath(const std::string &path) {
        for (auto &source: audioSources) {
            source->stop();
            source->clearBuffer();
        }
        sourceIndex = 0;
        audioSample = engine::loadAudioBuffer(path, *audioContext);
    }

private:
    std::unique_ptr<engine::AudioDevice> audioDevice;
    std::unique_ptr<engine::AudioContext> audioContext;
    std::unique_ptr<engine::AudioBuffer> audioSample;

    int sourceIndex = 0;
    std::vector<std::unique_ptr<engine::AudioSource>> audioSources;
};

#endif //METRONOME_SAMPLEPLAYER_HPP
