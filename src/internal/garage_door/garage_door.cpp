#include <internal/garage_door/garage_door.h>

//////////////////// Constructor
GarageDoor::GarageDoor() {
    m_audio = new Audio();
    m_doorIsOpened = false;

    m_doorStatusTimer = new Timer(120 * 1000);
    m_doorStatusTimer->start();
}

//////////////////// Public methods implementation
void GarageDoor::openDoor() {
    m_doorIsOpened = true;
    lg->info("garage door was opened", __FILE__, __LINE__);
    if (m_audio->play(DOOR_OPENED))
        lg->debug("garage door open adio is now playing", __FILE__, __LINE__);
    else
        lg->error("fail to play garage door open adio", __FILE__, __LINE__);
}
void GarageDoor::closeDoor() {
    m_doorIsOpened = false;
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

void GarageDoor::loop() {
    if (m_doorStatusTimer->isTime())
        sendDoorStatusRefresh();

    if (m_doorIsOpened && !m_audio->isPlaying()) {
        if (m_audio->play(DOOR_OPENED))
            lg->debug("garage door open adio is now playing", __FILE__, __LINE__);
        else
            lg->error("fail to play garage door open adio", __FILE__, __LINE__);
    }
    
    m_audio->loop();
}

//////////////////// Private methods implementation
bool GarageDoor::sendDoorStatusRefresh() {
    if (_mqtt == nullptr)
        return false;

    return _mqtt->publish("topic-garage-door-cmd", "REFRESH");
}
