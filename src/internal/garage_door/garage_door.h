#ifndef garage_door_h
#define garage_door_h

#include <internal/garage_door/audio.h>
#include <internal/garage_door/models.h>

#include <internal/core/mqtt_connection.h>
#include <internal/core/timer.h>

class GarageDoor {
    private:
        uint8_t m_doorOpenWarningTime;
        uint16_t m_refreshDoorStatusTime;
        Audio *m_audio = nullptr;

        const uint8_t DEFAULT_DOOR_OPEN_WARNING_TIME = 45;
        const uint8_t DEFAULT_REFRESH_DOOR_STATUS_TIME = 120;

        // This timer is for request periodically the garage door status
        Timer *m_tmrRefreshdoorStatus;
        bool m_doorIsOpened;

        // This timer counts the time to wait to raise the opened door warning
        Timer *m_tmrDoorOpenWarning;

        bool sendDoorStatusRefresh();
        bool playOpenDoorAudio();

    public:
        GarageDoor(int8_t doorOpenWarningTime = -1, int16_t refreshDoorStatusTime = -1);

        void openDoor();
        void closeDoor();
        bool ringDoorbell();
        void setDoorOpenWarningTime(uint8_t doorOpenWarningTime);
        uint8_t getDoorOpenWarningTime();
        void setRefreshDoorStatusTime(uint16_t refreshDoorStatusTime);
        uint16_t getRefreshDoorStatusTime();

        void loop();
};

#endif
