#include <internal/garage_door/audio.h>

//////////////////// Constructor
Audio::Audio() {
    m_currentAudio = NO_SOUND;
}

//////////////////// Public methods implementation
bool Audio::play(uint8_t audioType) {
    if (audioType != DOOR_OPENED && audioType != DOOR_BELL)
        return false;
    
    if (isPlaying())
        return true;

    if (mp3 != nullptr)
        stop();

    switch (audioType) {
    case DOOR_OPENED:
        file = new AudioFileSourceLittleFS("/audio/door_opened.mp3");
        break;
    case DOOR_BELL:
        file = new AudioFileSourceLittleFS("/audio/doorbell.mp3");
        break;
    default:
        return false;
    }
    m_currentAudio = audioType;
    out = new AudioOutputI2S(0, 1);
    mp3 = new AudioGeneratorMP3();
    return mp3->begin(file, out);
}

void Audio::stop() {
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
    m_currentAudio = NO_SOUND;
}

bool Audio::isPlaying() {
    if (mp3 == nullptr)
        return false;

    return mp3->isRunning();
}

uint8_t Audio::currentAudio() {
    return m_currentAudio;
}

void Audio::loop() {
    if (mp3 == nullptr)
        return;

    if (mp3->isRunning()) {
        if (!mp3->loop())
            stop();
    } else {
        stop();
    }
}

//////////////////// Private methods implementation
