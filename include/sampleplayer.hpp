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
    SamplePlayer() {
        audioDevice = engine::AudioDevice::createDevice(engine::OpenAL);
        audioContext = audioDevice->createContext();
        audioContext->makeCurrent();
    }

    explicit SamplePlayer(const std::string &samplePath) {
        audioDevice = engine::AudioDevice::createDevice(engine::OpenAL);
        audioContext = audioDevice->createContext();
        audioContext->makeCurrent();
        setSamplePath(samplePath);
    }

    void play() {
        if (audioSample == nullptr) {
            throw std::runtime_error("No sample loaded");
        }

        auto source = audioContext->createSource();
        source->setLooping(false);
        source->setBuffer(*audioSample);
        source->play();

        std::set<int> del;
        for (auto &pair: audioSources) {
            if (pair.second->getState() == engine::AudioSource::STOPPED) {
                del.insert(pair.first);
            }
        }

        for (auto &i: del)
            audioSources.erase(i);

        if (!sourceCounterCache.empty()) {
            auto id = *sourceCounterCache.begin();
            sourceCounterCache.erase(sourceCounterCache.begin());
            audioSources[id] = std::move(source);
        } else {
            if (sourceCounter == std::numeric_limits<int>::max())
                throw std::runtime_error("Counter overflow");
            audioSources[sourceCounter++] = std::move(source);
        }
    }

    void setSamplePath(const std::string &path) {
        audioSources.clear();
        sourceCounter = 0;
        sourceCounterCache.clear();
        audioSample = engine::loadAudioBuffer(path, *audioContext);
    }

private:
    std::unique_ptr<engine::AudioDevice> audioDevice;
    std::unique_ptr<engine::AudioContext> audioContext;
    std::unique_ptr<engine::AudioBuffer> audioSample;
    int sourceCounter = 0;
    std::set<int> sourceCounterCache;
    std::map<int, std::unique_ptr<engine::AudioSource>> audioSources;
};

#endif //METRONOME_SAMPLEPLAYER_HPP
