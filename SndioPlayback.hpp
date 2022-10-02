#pragma once

#include <cstdint>
#include <sndio.h>

namespace sndio {
    struct AudioFormat {
        uint16_t bitsPerSample;
        uint32_t sampleRate;
        uint16_t channels;

        AudioFormat(uint16_t bitsPerSample_, uint32_t sampleRate_, uint16_t channels_);
    };

    class SndioPlaybackDevice {
    public:
        SndioPlaybackDevice() = default;
        SndioPlaybackDevice(SndioPlaybackDevice const& other) = delete;
        SndioPlaybackDevice & operator=(SndioPlaybackDevice const& other) = delete;
        ~SndioPlaybackDevice();

        bool Open(char const* deviceName = SIO_DEVANY, int nonblockFlag = 0);
        void Close();
        bool Opened() const;

        bool Start(AudioFormat const& format);
        void Stop();
        bool Started() const;

        bool Play(void const* signal, size_t size);

    private:
        sio_hdl * m_handle = nullptr;
        bool m_started = false;
    };
}
