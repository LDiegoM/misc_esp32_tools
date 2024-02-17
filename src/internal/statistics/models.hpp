#ifndef statistics_models_h
#define statistics_models_h

#include <Arduino.h>

struct date_values_count_t {
    String period;
    int32_t eventsCount;
    int32_t warningsCount;
};

struct last_opened_duration_t {
    String openedTime;
    bool warnTriggered;
    bool wasClosed;
    String openedDuration;
};

#endif
