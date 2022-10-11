#pragma once

#include <cstdint>
#include <sndio.h>

namespace sndio {
    struct AudioFormat {
        uint16_t bitsPerSample;
        uint32_t sampleRate;
        uint16_t playbackChannels;
        uint16_t recorderChannels;

        AudioFormat(uint16_t bitsPerSample_, uint32_t sampleRate_, uint16_t channels_);
        AudioFormat(uint16_t bitsPerSample_, uint32_t sampleRate_, uint16_t playbackChannels_,
                                                                   uint16_t recorderChannels_);
    };

    class DeviceMode {
    public:
        DeviceMode(DeviceMode const& other);
        DeviceMode & operator=(DeviceMode const& other);
        DeviceMode operator|(DeviceMode const& other) const;
        bool Verify(DeviceMode const& mode) const;

        DeviceMode static const nought;
        DeviceMode static const nonblock;
        DeviceMode static const playback;
        DeviceMode static const recorder;

    private:
        using Mode = uint32_t;
        Mode m_mode = 0;

        DeviceMode(Mode mode);
    };

    class BaseDevice {
    public:
        BaseDevice(BaseDevice const& other) = delete;
        BaseDevice & operator=(BaseDevice const& other) = delete;
        virtual ~BaseDevice();

        bool Open(DeviceMode mode, char const* deviceName);
        void Close();
        bool Opened() const;
        bool VerifyMode(DeviceMode const& mode) const;

        bool Start(AudioFormat const& format);
        void Stop();
        bool Started() const;

    protected:
        BaseDevice() = default;

        sio_hdl * Handler();

    private:
        DeviceMode m_mode = DeviceMode::nought;
        sio_hdl * m_handler = nullptr;
        bool m_started = false;
    };

    class PlaybackDevice : public virtual BaseDevice {
    public:
        PlaybackDevice() = default;
        ~PlaybackDevice() override = default;

        bool Open(char const* deviceName = SIO_DEVANY, bool nonblockFlag = false);
        size_t Play(void const* signal, size_t size);
    };

    class RecorderDevice : public virtual BaseDevice {
    public:
        RecorderDevice() = default;
        ~RecorderDevice() override = default;

        bool Open(char const* deviceName = SIO_DEVANY, bool nonblockFlag = false);
        size_t Record(void * signal, size_t size);
    };

    class Device : public PlaybackDevice, public RecorderDevice {
    public:
        Device() = default;
        ~Device() override = default;

        bool Open(DeviceMode mode, char const* deviceName = SIO_DEVANY);
    };
}
