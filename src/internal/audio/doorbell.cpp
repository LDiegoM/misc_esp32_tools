#include <internal/audio/doorbell.h>

//////////////////// Constructor
Doorbell::Doorbell() {}

//////////////////// Public methods implementation
bool Doorbell::play() {
    if (mp3 == nullptr) {
        file = new AudioFileSourceLittleFS("/audio/doorbell.mp3");
        out = new AudioOutputI2S(0, 1);
        mp3 = new AudioGeneratorMP3();
    }

    return mp3->begin(file, out);
}

void Doorbell::stop() {
    if (mp3 != nullptr) {
        mp3->stop();
        free(mp3);
        mp3 = nullptr;
    }

    if (file != nullptr) {
        free(file);
        file = nullptr;
    }

    if (out != nullptr) {
        free(out);
        out = nullptr;
    }
}

bool Doorbell::isPlaying() {
    if (mp3 == nullptr)
        return false;

    return mp3->isRunning();
}

void Doorbell::loop() {
    if (mp3 != nullptr) {
        if (mp3->isRunning()) {
            if (!mp3->loop())
                stop();
        } else {
            stop();
        }
    }
}

//////////////////// Private methods implementation
