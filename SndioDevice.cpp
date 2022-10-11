#include "SndioDevice.hpp"

sndio::AudioFormat::AudioFormat(uint16_t bitsPerSample_,
                                uint32_t sampleRate_,
                                uint16_t channels_)
{
    bitsPerSample = bitsPerSample_;
    sampleRate = sampleRate_;
    playbackChannels = recorderChannels = channels_;
}

sndio::AudioFormat::AudioFormat(uint16_t bitsPerSample_,
                                uint32_t sampleRate_,
                                uint16_t playbackChannels_,
                                uint16_t recorderChannels_)
{
    bitsPerSample = bitsPerSample_;
    sampleRate = sampleRate_;
    playbackChannels = playbackChannels_;
    recorderChannels = recorderChannels_;
}

sndio::DeviceMode const sndio::DeviceMode::nought(0);
sndio::DeviceMode const sndio::DeviceMode::nonblock(1 << 0);
sndio::DeviceMode const sndio::DeviceMode::playback(1 << 1);
sndio::DeviceMode const sndio::DeviceMode::recorder(1 << 2);

sndio::DeviceMode::DeviceMode(DeviceMode const& other) {
    m_mode = other.m_mode;
}

sndio::DeviceMode::DeviceMode(Mode mode) {
    m_mode = mode;
}

sndio::DeviceMode & sndio::DeviceMode::operator=(DeviceMode const& other) {
    m_mode = other.m_mode;
    return *this;
}

sndio::DeviceMode sndio::DeviceMode::operator|(DeviceMode const& other) const {
    return DeviceMode(m_mode | other.m_mode);
}

bool sndio::DeviceMode::Verify(DeviceMode const& mode) const {
    return (m_mode & mode.m_mode) == mode.m_mode;
}

sndio::BaseDevice::~BaseDevice() {
    Close();
}

bool sndio::BaseDevice::Open(DeviceMode mode, char const* deviceName) {
    Close();

    m_mode = mode;
    unsigned sioMode = 0;
    if (VerifyMode(DeviceMode::playback)) {
        sioMode |= SIO_PLAY;
    }
    if (VerifyMode(DeviceMode::recorder)) {
        sioMode |= SIO_REC;
    }
    bool const nonblockFlag = VerifyMode(DeviceMode::nonblock);

    m_handler = sio_open(deviceName, sioMode, nonblockFlag);

    return Opened();
}

void sndio::BaseDevice::Close() {
    if (!Opened()) {
        return;
    }

    sio_close(m_handler);

    m_mode = DeviceMode::nought;
    m_handler = nullptr;
    m_started = false;
}

bool sndio::BaseDevice::Opened() const {
    return m_handler != nullptr;
}

bool sndio::BaseDevice::VerifyMode(DeviceMode const& mode) const {
    return m_mode.Verify(mode);
}

bool sndio::BaseDevice::Start(AudioFormat const& format) {
    if (!Opened()) {
        return false;
    }

    Stop();

    sio_par params;
    sio_initpar(&params);
    params.bits = format.bitsPerSample;
    params.rate = format.sampleRate;
    params.pchan = format.playbackChannels;
    params.rchan = format.recorderChannels;
    if (!sio_setpar(m_handler, &params)) {
        return false;
    }

    m_started = sio_start(m_handler);

    return Started();
}

void sndio::BaseDevice::Stop() {
    if (!Started()) {
        return;
    }

    sio_stop(m_handler);

    m_started = false;
}

bool sndio::BaseDevice::Started() const {
    return Opened() && m_started;
}

sio_hdl * sndio::BaseDevice::Handler() {
    return m_handler;
}

bool sndio::PlaybackDevice::Open(char const* deviceName, bool nonblockFlag) {
    DeviceMode mode = nonblockFlag ? DeviceMode::playback | DeviceMode::nonblock
                                   : DeviceMode::playback;
    return BaseDevice::Open(mode, deviceName);
}

size_t sndio::PlaybackDevice::Play(void const* signal, size_t size) {
    if (!VerifyMode(DeviceMode::playback) || !Started()) {
        return static_cast<size_t>(-1);
    }

    sio_hdl * handler = Handler();
    if (VerifyMode(DeviceMode::nonblock)) {
        return sio_write(handler, signal, size);
    }

    uint8_t const* buffer = reinterpret_cast<uint8_t const*>(signal);
    for (size_t i = 0; i < size; i += sio_write(handler, buffer + i, size - i))
        ;
    return size;
}

bool sndio::RecorderDevice::Open(char const* deviceName, bool nonblockFlag) {
    DeviceMode mode = nonblockFlag ? DeviceMode::recorder | DeviceMode::nonblock
                                   : DeviceMode::recorder;
    return BaseDevice::Open(mode, deviceName);
}

size_t sndio::RecorderDevice::Record(void * signal, size_t size) {
    if (!VerifyMode(DeviceMode::recorder) || !Started()) {
        return static_cast<size_t>(-1);
    }

    sio_hdl * handler = Handler();
    if (VerifyMode(DeviceMode::nonblock)) {
        return sio_read(handler, signal, size);
    }

    uint8_t * buffer = reinterpret_cast<uint8_t *>(signal);
    for (size_t i = 0; i < size; i += sio_read(handler, buffer + i, size - i))
        ;
    return size;
}

bool sndio::Device::Open(DeviceMode mode, char const* deviceName) {
    return BaseDevice::Open(mode, deviceName);
}
