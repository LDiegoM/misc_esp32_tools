#include <internal/garage_door/garage_door.h>

//////////////////// Constructor
GarageDoor::GarageDoor(int8_t doorOpenWarningTime, int16_t refreshDoorStatusTime) {
    m_doorIsOpened = false;

    if (doorOpenWarningTime < 0)
        m_doorOpenWarningTime = DEFAULT_DOOR_OPEN_WARNING_TIME;
    else
        m_doorOpenWarningTime = doorOpenWarningTime;

    if (refreshDoorStatusTime < 0)
        m_refreshDoorStatusTime = DEFAULT_REFRESH_DOOR_STATUS_TIME;
    else
        m_refreshDoorStatusTime = refreshDoorStatusTime;
    
    m_tmrRefreshdoorStatus = new Timer(m_refreshDoorStatusTime * 1000);
    if (m_refreshDoorStatusTime > 0)
        m_tmrRefreshdoorStatus->start();
}

//////////////////// Public methods implementation
void GarageDoor::openDoor() {
    m_doorIsOpened = true;
    lg->info("garage door was opened. starting warning timer", __FILE__, __LINE__);
}
void GarageDoor::closeDoor() {
    m_doorIsOpened = false;
    lg->info("garage door was closed", __FILE__, __LINE__);
}
bool GarageDoor::ringDoorbell() {
    return true;
}

void GarageDoor::setDoorOpenWarningTime(uint8_t doorOpenWarningTime) {
    m_doorOpenWarningTime = doorOpenWarningTime;
}
uint8_t GarageDoor::getDoorOpenWarningTime() {
    return m_doorOpenWarningTime;
}

void GarageDoor::setRefreshDoorStatusTime(uint16_t refreshDoorStatusTime) {
    if (m_tmrRefreshdoorStatus->isRunning())
        m_tmrRefreshdoorStatus->stop();
    free(m_tmrRefreshdoorStatus);

    m_refreshDoorStatusTime = refreshDoorStatusTime;
    m_tmrRefreshdoorStatus = new Timer(m_refreshDoorStatusTime * 1000);

    if (m_refreshDoorStatusTime > 0)
        m_tmrRefreshdoorStatus->start();
}
uint16_t GarageDoor::getRefreshDoorStatusTime() {
    return m_refreshDoorStatusTime;
}


void GarageDoor::loop() {
    if (m_tmrRefreshdoorStatus->isRunning() && m_tmrRefreshdoorStatus->isTime())
        sendDoorStatusRefresh();
}

//////////////////// Private methods implementation
bool GarageDoor::sendDoorStatusRefresh() {
    if (_mqtt == nullptr)
        return false;

    return _mqtt->publish("topic-door-cmd", "RESEND");
}

bool GarageDoor::playOpenDoorAudio() {
    return true;
}
