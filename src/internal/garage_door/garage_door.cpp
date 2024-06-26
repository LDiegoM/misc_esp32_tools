#include <internal/garage_door/garage_door.h>

//////////////////// Constructor
GarageDoor::GarageDoor(int8_t doorOpenWarningTime, int16_t refreshDoorStatusTime) {
    m_audio = new Audio();
    m_doorIsOpened = false;

    if (doorOpenWarningTime < 0)
        m_doorOpenWarningTime = DEFAULT_DOOR_OPEN_WARNING_TIME;
    else
        m_doorOpenWarningTime = doorOpenWarningTime;

    if (refreshDoorStatusTime < 0)
        m_refreshDoorStatusTime = DEFAULT_REFRESH_DOOR_STATUS_TIME;
    else
        m_refreshDoorStatusTime = refreshDoorStatusTime;
    
    m_tmrDoorOpenWarning = new Timer(m_doorOpenWarningTime * 1000);

    m_tmrRefreshdoorStatus = new Timer(m_refreshDoorStatusTime * 1000);
    if (m_refreshDoorStatusTime > 0)
        m_tmrRefreshdoorStatus->start();
}

//////////////////// Public methods implementation
void GarageDoor::openDoor() {
    m_doorIsOpened = true;
    lg->info("garage door was opened. starting warning timer", __FILE__, __LINE__);
    m_tmrDoorOpenWarning->start();
}
void GarageDoor::closeDoor() {
    m_doorIsOpened = false;
    m_tmrDoorOpenWarning->stop();
    lg->info("garage door was closed", __FILE__, __LINE__);
    if (m_audio->isPlaying() && m_audio->currentAudio() == DOOR_OPENED)
        m_audio->stop();
}
bool GarageDoor::ringDoorbell() {
    if (m_audio->isPlaying())
        return false;

    bool ok = m_audio->play(DOOR_BELL);
    if (ok)
        lg->info("doorbell is now ringing", __FILE__, __LINE__);
    else
        lg->error("fail to ring doorbell", __FILE__, __LINE__);
    return ok;
}

void GarageDoor::setDoorOpenWarningTime(uint8_t doorOpenWarningTime) {
    bool flgTimerIsRunning = m_tmrDoorOpenWarning->isRunning();
    if (flgTimerIsRunning)
        m_tmrDoorOpenWarning->stop();
    free(m_tmrDoorOpenWarning);

    m_doorOpenWarningTime = doorOpenWarningTime;
    m_tmrDoorOpenWarning = new Timer(m_doorOpenWarningTime * 1000);

    if (flgTimerIsRunning)
        m_tmrDoorOpenWarning->start();
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

    if (m_doorIsOpened && !m_audio->isPlaying() && m_tmrDoorOpenWarning->isTime()) {
        playOpenDoorAudio();
        m_tmrDoorOpenWarning->stop();
    }
    
    m_audio->loop();
}

//////////////////// Private methods implementation
bool GarageDoor::sendDoorStatusRefresh() {
    if (_mqtt == nullptr)
        return false;

    return _mqtt->publish("topic-door-cmd", "RESEND");
}

bool GarageDoor::playOpenDoorAudio() {
    lg->warn("garage door is opened for more than warning configured time", __FILE__, __LINE__,
        lg->newTags()
            ->add("doorOpenWarningTime", String(m_doorOpenWarningTime))
    );
    bool ok = m_audio->play(DOOR_OPENED);
    if (ok)
        lg->debug("garage door open audio is now playing", __FILE__, __LINE__);
    else
        lg->error("fail to play garage door open audio", __FILE__, __LINE__);
    return ok;
}
