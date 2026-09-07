#ifndef PT_AUDIO_HPP
#define PT_AUDIO_HPP

#include <SDL2/SDL.h>
#include <cmath>
#include <atomic>
#include <chrono>
#include <memory>
#include <algorithm>
#include <thread>

namespace pt_audio {

constexpr double PI = 3.14159265358979323846;
constexpr int SAMPLE_RATE = 44100;
constexpr int CHANNELS = 2;
constexpr int BUFFER_SIZE = 2048;

enum class WaveType {
    SINE,
    SQUARE,
    TRIANGLE,
    SAWTOOTH
};

class WaveGenerator {
private:
    double phase = 0.0;
    double frequency = 440.0;
    double amplitude = 0.5;
    WaveType wave_type = WaveType::SINE;
    std::atomic<bool> active{true};

public:
    void set_frequency(double freq) { frequency = freq; }
    void set_amplitude(double amp) { amplitude = std::clamp(amp, 0.0, 1.0); }
    void set_wave_type(WaveType type) { wave_type = type; }
    void stop() { active = false; }
    void reset() { phase = 0.0; active = true; }
    
    double generate() {
        if (!active) return 0.0;
        
        double sample = 0.0;
        
        switch (wave_type) {
            case WaveType::SINE:
                sample = amplitude * sin(2.0 * PI * phase);
                break;
            case WaveType::SQUARE:
                sample = amplitude * (phase < 0.5 ? 1.0 : -1.0);
                break;
            case WaveType::TRIANGLE:
                sample = amplitude * (2.0 * fabs(2.0 * phase - 1.0) - 1.0);
                break;
            case WaveType::SAWTOOTH:
                sample = amplitude * (2.0 * phase - 1.0);
                break;
        }
        
        phase += frequency / SAMPLE_RATE;
        if (phase >= 1.0) phase -= 1.0;
        
        return sample;
    }
};

class AudioPlayer {
private:
    SDL_AudioDeviceID device_id = 0;
    SDL_AudioSpec audio_spec;
    std::shared_ptr<WaveGenerator> generator;
    std::atomic<bool> is_playing{false};
    std::atomic<bool> should_stop{false};
    bool sdl_initialized = false;

    static void audio_callback(void* userdata, Uint8* stream, int len) {
        AudioPlayer* player = static_cast<AudioPlayer*>(userdata);
        if (!player || !player->is_playing || !player->generator) {
            SDL_memset(stream, 0, len);
            return;
        }

        Sint16* samples = reinterpret_cast<Sint16*>(stream);
        int sample_count = len / (sizeof(Sint16) * CHANNELS);
        
        for (int i = 0; i < sample_count; i++) {
            double value = player->generator->generate();
            Sint16 sample = static_cast<Sint16>(value * 32767.0);
            samples[i * CHANNELS] = sample;
            samples[i * CHANNELS + 1] = sample;
        }
    }

    bool init_audio() {
        if (!sdl_initialized) {
            if (SDL_Init(SDL_INIT_AUDIO) < 0) {
                return false;
            }
            sdl_initialized = true;
        }

        SDL_AudioSpec want;
        SDL_zero(want);
        want.freq = SAMPLE_RATE;
        want.format = AUDIO_S16SYS;
        want.channels = CHANNELS;
        want.samples = BUFFER_SIZE;
        want.callback = audio_callback;
        want.userdata = this;

        device_id = SDL_OpenAudioDevice(NULL, 0, &want, &audio_spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
        if (device_id == 0) {
            return false;
        }

        return true;
    }

    void cleanup() {
        if (device_id != 0) {
            SDL_CloseAudioDevice(device_id);
            device_id = 0;
        }
        if (sdl_initialized) {
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
            sdl_initialized = false;
        }
    }

public:
    AudioPlayer() {
        generator = std::make_shared<WaveGenerator>();
    }

    ~AudioPlayer() {
        stop();
        cleanup();
    }

    void set_wave_type(WaveType type) {
        if (generator) {
            generator->set_wave_type(type);
        }
    }

    void set_amplitude(double amplitude) {
        if (generator) {
            generator->set_amplitude(amplitude);
        }
    }

    void play_async(int frequency, int duration_ms, double amplitude = 0.5) {
        if (is_playing) {
            stop();
        }

        if (device_id == 0) {
            if (!init_audio()) {
                return;
            }
        }

        if (!generator) {
            generator = std::make_shared<WaveGenerator>();
        }
        generator->set_frequency(frequency);
        generator->set_amplitude(amplitude);
        generator->reset();

        is_playing = true;
        should_stop = false;
        SDL_PauseAudioDevice(device_id, 0);

        std::thread([this, duration_ms]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
            stop();
        }).detach();
    }

    void stop() {
        is_playing = false;
        should_stop = true;
        if (generator) {
            generator->stop();
        }
        if (device_id != 0) {
            SDL_PauseAudioDevice(device_id, 1);
        }
    }

    bool is_playing_note() const {
        return is_playing.load();
    }
};

class AudioManager {
private:
    AudioManager() = default;
    ~AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    std::unique_ptr<AudioPlayer> player;

public:
    static AudioManager& get_instance() {
        static AudioManager instance;
        return instance;
    }

    void init() {
        if (!player) {
            player = std::make_unique<AudioPlayer>();
        }
    }

    void play_async(int frequency, int duration_ms, double amplitude = 0.5) {
        init();
        player->play_async(frequency, duration_ms, amplitude);
    }
    
    void play_sec_async(int frequency, int duration_sec, double amplitude = 0.5) {
        init();
        player->play_async(frequency, duration_sec * 1000, amplitude);
    }
    
    void set_wave_type(WaveType type) {
        init();
        player->set_wave_type(type);
    }
    
    void set_amplitude(double amplitude) {
        init();
        player->set_amplitude(amplitude);
    }
    
    void stop() {
        if (player) {
            player->stop();
        }
    }

    bool is_playing() {
        if (player) {
            return player->is_playing_note();
        }
        return false;
    }
};

inline void play_tone_async(int frequency, int duration_sec) {
    AudioManager::get_instance().play_sec_async(frequency, duration_sec);
}

inline void play_tone_ms_async(int frequency, int duration_ms) {
    AudioManager::get_instance().play_async(frequency, duration_ms);
}

inline void play_wave_async(int frequency, int duration_ms, WaveType type = WaveType::SINE, double amplitude = 0.5) {
    auto& manager = AudioManager::get_instance();
    manager.set_wave_type(type);
    manager.set_amplitude(amplitude);
    manager.play_async(frequency, duration_ms);
}

inline bool is_playing() {
    return AudioManager::get_instance().is_playing();
}

inline void stop_audio() {
    AudioManager::get_instance().stop();
}

}

#endif