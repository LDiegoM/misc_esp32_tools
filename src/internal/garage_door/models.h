#ifndef garage_door_models_h
#define garage_door_models_h

#include <Arduino.h>

struct garage_door_t {
    uint8_t doorOpenWarningTime;
    uint16_t refreshDoorStatusTime;
};

#endif
