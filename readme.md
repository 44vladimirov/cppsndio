# C++ wrapper over [sndio](https://sndio.org) library

To use the wrapper simply
* clone the repository as git submodule,
* make `add_subdirectory` in your `CMakeLists.txt`,
* link your target with provided `cppsndio` CMake object library.

See [the alarm utility](https://github.com/44vladimirov/alarm) for example.

## Docs

The wrapper provides sndio virtual device `sndio::Device`.

```C++
class Device {
public:
    bool Open(DeviceMode mode, char const* deviceName = SIO_DEVANY);
    void Close();
    bool Opened() const;

    bool Start(AudioFormat const& format);
    void Stop();
    bool Started() const;

    size_t Play(void const* signal, size_t size);
    size_t Record(void * signal, size_t size);
    size_t RecordFull(void * signal, size_t size);
};
```

To use the device you should `Open` it first.
Device can be opened in different `sndio::DeviceMode`s.
Provided modes are
* `sndio::DeviceMode::playback`,
* `sndio::DeviceMode::recorder`,
* `sndio::DeviceMode::nonblock`.

You should use `operator|` to construct new modes from the specified.
For example, full-duplex mode can be constructed as follows.
```C++
sndio::DeviceMode::playback | sndio::DeviceMode::recorder
```

`sndio::PlaybackDevice` and `sndio::RecorderDevice` are synonyms for `sndio::Device` opened with
playback and recorder modes accordingly.

When you are ready for playing or recording, you should `Start` the process.
Device can be started in different `sndio::AudioFormat`s.

```C++
sndio::AudioFormat(uint16_t bitsPerSample_, uint32_t sampleRate_, uint16_t channels_);

sndio::AudioFormat(uint16_t bitsPerSample_, uint32_t sampleRate_, uint16_t playbackChannels_,
                                                                  uint16_t recorderChannels_);
```

With the started device you can `Play` or `Record`.
`Play` method blocks the caller until all specified data has been handled.
As well as `Record` blocks until some data has been recorded.
The behavior changes if `sndio::DeviceMode::nonblock` is set.
In that case the methods never block the caller.

In blocked mode `RecordFull` method is available.
It blocks the caller until specified buffer has been filled *fully*.

`Play`, `Record`, `RecordFull` return count of handled bytes on success or
`(size_t) -1` on failure.
