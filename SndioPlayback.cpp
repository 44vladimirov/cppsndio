#include "SndioPlayback.hpp"

sndio::AudioFormat::AudioFormat(uint16_t bitsPerSample_,
                                uint32_t sampleRate_,
                                uint16_t channels_)
{
    bitsPerSample = bitsPerSample_;
    sampleRate = sampleRate_;
    channels = channels_;
}

sndio::SndioPlaybackDevice::~SndioPlaybackDevice() {
    Close();
}

bool sndio::SndioPlaybackDevice::Open(char const* deviceName, int nonblockFlag) {
    Close();

    m_handle = sio_open(deviceName, SIO_PLAY, nonblockFlag);

    return Opened();
}

void sndio::SndioPlaybackDevice::Close() {
    if (!Opened()) {
        return;
    }

    sio_close(m_handle);

    m_handle = nullptr;
    m_started = false;
}

bool sndio::SndioPlaybackDevice::Opened() const {
    return m_handle != nullptr;
}

bool sndio::SndioPlaybackDevice::Start(AudioFormat const& format) {
    if (!Opened()) {
        return false;
    }

    Stop();

    sio_par params;
    sio_initpar(&params);
    params.bits = format.bitsPerSample;
    params.rate = format.sampleRate;
    params.rchan = format.channels;
    if (!sio_setpar(m_handle, &params)) {
        return false;
    }

    m_started = sio_start(m_handle);

    return Started();
}

void sndio::SndioPlaybackDevice::Stop() {
    if (!Started()) {
        return;
    }

    sio_stop(m_handle);

    m_started = false;
}

bool sndio::SndioPlaybackDevice::Started() const {
    return Opened() && m_started;
}

bool sndio::SndioPlaybackDevice::Play(void const* signal, size_t size) {
    if (!Started()) {
        return false;
    }

    uint8_t const* buffer = reinterpret_cast<uint8_t const*>(signal);
    for (size_t i = 0; i < size; i += sio_write(m_handle, buffer + i, size - i))
        ;

    return true;
}
