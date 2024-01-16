#ifndef doorbell_h
#define doorbell_h

#include <AudioFileSourceLittleFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>

class Doorbell {
    private:
        AudioGeneratorMP3 *mp3 = nullptr;
        AudioFileSourceLittleFS *file = nullptr;
        AudioOutputI2S *out = nullptr;

    public:
        Doorbell();

        bool play();
        void stop();
        bool isPlaying();

        void loop();
};

#endif
