#ifndef doorbell_h
#define doorbell_h

#include <AudioFileSourceLittleFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>

#define NO_SOUND    -1
#define DOOR_OPENED 0
#define DOOR_BELL   1

class Audio {
    private:
        uint8_t m_currentAudio;
        AudioGeneratorMP3 *mp3 = nullptr;
        AudioFileSourceLittleFS *file = nullptr;
        AudioOutputI2S *out = nullptr;

    public:
        Audio();

        bool play(uint8_t audioType);
        void stop();
        bool isPlaying();
        uint8_t currentAudio();

        void loop();
};

#endif
