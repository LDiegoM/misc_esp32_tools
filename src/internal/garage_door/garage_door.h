#ifndef garage_door_h
#define garage_door_h

#include <internal/garage_door/audio.h>

#include <internal/core/mqtt_connection.h>
#include <internal/core/timer.h>

class GarageDoor {
    private:
        Audio *m_audio = nullptr;

        // This timer is for request periodically the garage door status
        Timer *m_doorStatusTimer;
        bool m_doorIsOpened;

        bool sendDoorStatusRefresh();

    public:
        GarageDoor();

        void openDoor();
        void closeDoor();
        bool ringDoorbell();

        void loop();
};

#endif
