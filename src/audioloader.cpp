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

#include "audioloader.hpp"

#include <string>
#include <sndfile.h>

#include <stdexcept>
#include <limits>
#include <cstring>
#include <fstream>

namespace engine {
    class Audio {
    public:
        std::vector<uint8_t> buffer;
        AudioFormat format;
        unsigned int frequency;
    };

    struct LibSndBuffer {
        std::string data;
        size_t pos;
    };

    sf_count_t sf_vio_get_filelen(void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        return static_cast<sf_count_t>(buffer->data.size());
    }

    sf_count_t sf_vio_seek(sf_count_t offset, int whence, void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        buffer->pos = offset;
        return static_cast<sf_count_t >(buffer->pos);
    }

    sf_count_t sf_vio_read(void *ptr, sf_count_t count, void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        sf_count_t ret;
        for (ret = 0; ret < count && buffer->pos + ret < buffer->data.size(); ret++) {
            static_cast<char *>(ptr)[ret] = buffer->data.at(buffer->pos + ret);
        }
        buffer->pos += ret;
        return ret;
    }

    sf_count_t sf_vio_write(const void *ptr, sf_count_t count, void *user_data) {
        throw std::runtime_error("VIO Write");
    }

    sf_count_t sf_vio_tell(void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        return static_cast<sf_count_t >(buffer->pos);
    }

    static Audio processSndFile(SNDFILE *sndfile, const SF_INFO &sfinfo) {
        if (sfinfo.frames<1
                          || sfinfo.frames>(sf_count_t)(std::numeric_limits<int>::max() / sizeof(short)) /
            sfinfo.channels) {
            sf_close(sndfile);
            throw std::runtime_error("Bad sample count in audio buffer");
        }

        Audio ret;

        if (sfinfo.channels == 1) {
            ret.format = MONO16;
        } else if (sfinfo.channels == 2) {
            ret.format = STEREO16;
        } else if (sfinfo.channels == 3) {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) {
                ret.format = BFORMAT2D_16;
            } else {
                sf_close(sndfile);
                throw std::runtime_error("Unsupported channel count: " + std::to_string(sfinfo.channels));
            }
        } else if (sfinfo.channels == 4) {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) {
                ret.format = BFORMAT3D_16;
            } else {
                sf_close(sndfile);
                throw std::runtime_error("Unsupported channel count: " + std::to_string(sfinfo.channels));
            }
        } else {
            sf_close(sndfile);
            throw std::runtime_error("Unsupported channel count: " + std::to_string(sfinfo.channels));
        }

        ret.frequency = sfinfo.samplerate;

        auto *buff = static_cast<short *>(malloc((size_t) (sfinfo.frames * sfinfo.channels) * sizeof(short)));

        sf_count_t num_frames = sf_readf_short(sndfile, buff, sfinfo.frames);
        if (num_frames < 1) {
            free(buff);
            sf_close(sndfile);
            throw std::runtime_error("Failed to read samples from audio data");
        }

        sf_count_t num_bytes = num_frames * sfinfo.channels * sizeof(short);

        ret.buffer.resize(num_bytes);

        std::memcpy(ret.buffer.data(), buff, num_bytes);

        free(buff);

        sf_close(sndfile);

        return ret;
    }

    static Audio readAudio(const std::string &buf) {
        SF_VIRTUAL_IO virtio;
        virtio.get_filelen = &sf_vio_get_filelen;
        virtio.seek = &sf_vio_seek;
        virtio.read = &sf_vio_read;
        virtio.write = &sf_vio_write;
        virtio.tell = &sf_vio_tell;

        LibSndBuffer buffer{buf, 0};
        SF_INFO sfinfo;
        SNDFILE *sndfile = sf_open_virtual(&virtio, SFM_READ, &sfinfo, &buffer);
        if (!sndfile) {
            auto err = sf_strerror(sndfile);
            throw std::runtime_error("Failed to open audio buffer\nError:" + std::string(err));
        }
        return processSndFile(sndfile, sfinfo);
    }

    static Audio readAudioFile(const std::string &path) {
        SF_INFO sfinfo;
        SNDFILE *sndfile = sf_open(path.c_str(), SFM_READ, &sfinfo);
        if (!sndfile) {
            auto err = sf_strerror(sndfile);
            throw std::runtime_error("Failed to open audio file at " + path + "\nError: " + std::string(err));
        }
        return processSndFile(sndfile, sfinfo);
    }

    std::unique_ptr<AudioBuffer> loadAudioBuffer(const std::string &path, AudioContext &context) {
        auto audio = readAudioFile(path);
        auto ret = context.createBuffer();
        ret->upload(audio.buffer, audio.format, audio.frequency);
        return std::move(ret);
    }

    std::unique_ptr<AudioBuffer> loadAudioBufferData(const std::string &data, AudioContext &context) {
        auto audio = readAudio(data);
        auto ret = context.createBuffer();
        ret->upload(audio.buffer, audio.format, audio.frequency);
        return std::move(ret);
    }
}