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

#ifndef METRONOME_BEATGENERATOR_HPP
#define METRONOME_BEATGENERATOR_HPP

#include <chrono>
#include <set>

class BeatGenerator {
public:
    void reset() {
        lastUpdate = std::chrono::high_resolution_clock::now();
        accumulator = targetDuration;
    }

    std::chrono::high_resolution_clock::duration update() {
        auto now = std::chrono::high_resolution_clock::now();
        accumulator += now - lastUpdate;
        std::chrono::high_resolution_clock::duration ret;
        if (accumulator >= targetDuration) {
            accumulator = std::chrono::high_resolution_clock::duration(0);
            ret = std::chrono::high_resolution_clock::duration(0);
        } else {
            ret = targetDuration - accumulator;
        }
        lastUpdate = now;
        return ret;
    }

    void setBPM(uint32_t value) {
        targetDuration = std::chrono::high_resolution_clock::duration(static_cast<int64_t>(NANOSECONDS_PER_MINUTE)
                                                                      / static_cast<int64_t>(value));
    }

private:
    static const uint64_t NANOSECONDS_PER_MINUTE = 60000000000;

    std::chrono::high_resolution_clock::duration targetDuration;
    std::chrono::high_resolution_clock::time_point lastUpdate;
    std::chrono::high_resolution_clock::duration accumulator;
};

#endif //METRONOME_BEATGENERATOR_HPP