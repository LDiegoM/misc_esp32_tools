#ifndef logging_h
#define logging_h

#include <vector>
#include <internal/platform/storage.h>
#include <internal/platform/date_time.h>

#define LOG_LEVEL_DEBUG   0
#define LOG_LEVEL_INFO    1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR   3

extern const char* LOGGING_FILE;

struct logTag_t {
    String name;
    String value;
};

class LogTags {
    private:
        std::vector<logTag_t> m_tags;

        String tagToString(logTag_t tag);

    public:
        LogTags();
        ~LogTags();

        LogTags* add(String name, String value);
        String toString();
};

class Logging {
    private:
        uint8_t m_level;
        Storage *m_storage;
        DateTime *m_dt;

        String getDateTime();
        String getLogLevel(uint8_t level);
        String getFullData(String msg, uint8_t level);
        String getFullData(String msg, uint8_t level, LogTags *tags);
        void writeData(String fullData);

    public:
        Logging(uint8_t level, Storage *storage, DateTime *dt);
        Logging(uint8_t level, Storage *storage);

        void setLevel(uint8_t level);
        uint8_t getLevel();
        void setDateTime(DateTime *dt);

        LogTags* newTags();

        void debug(String msg);
        void info(String msg);
        void warn(String msg);
        void error(String msg);

        void debug(String msg, LogTags *tags);
        void info(String msg, LogTags *tags);
        void warn(String msg, LogTags *tags);
        void error(String msg, LogTags *tags);

        bool clear();
};

#endif
