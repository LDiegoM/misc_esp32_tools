#include <internal/platform/logging.h>

const char* LOGGING_FILE = "/logging/logs.txt";

//////////////////// Constructor
LogTags::LogTags() {
    m_tags.clear();
}

LogTags::~LogTags() {
    m_tags.clear();
}

Logging::Logging(uint8_t level, Storage *storage) {
    m_level = level;
    m_storage = storage;
    m_dt = nullptr;
}
Logging::Logging(uint8_t level, Storage *storage, DateTime *dt) {
    m_level = level;
    m_storage = storage;
    m_dt = dt;
}

//////////////////// Public methods implementation
LogTags* LogTags::add(String name, String value) {
    logTag_t tag = logTag_t{
        name = name,
        value = value
    };
    m_tags.push_back(tag);
    return this;
}

String LogTags::toString() {
    String s = "";
    for (int i = 0; i < m_tags.size(); i++) {
        s += tagToString(m_tags[i]);
    }
    return s;
}

void Logging::setLevel(uint8_t level) {
    m_level = level;
}
uint8_t Logging::getLevel() {
    return m_level;
}
void Logging::setDateTime(DateTime *dt) {
    m_dt = dt;
}

LogTags* Logging::newTags() {
    return new LogTags();
}

void Logging::debug(String msg) {
    if (m_level > LOG_LEVEL_DEBUG)
        return;
    writeData(getFullData(msg, LOG_LEVEL_DEBUG));
}
void Logging::info(String msg) {
    if (m_level > LOG_LEVEL_INFO)
        return;
    writeData(getFullData(msg, LOG_LEVEL_INFO));
}
void Logging::warn(String msg) {
    if (m_level > LOG_LEVEL_WARNING)
        return;
    writeData(getFullData(msg, LOG_LEVEL_WARNING));
}
void Logging::error(String msg) {
    writeData(getFullData(msg, LOG_LEVEL_ERROR));
}

void Logging::debug(String msg, LogTags *tags) {
    if (m_level > LOG_LEVEL_DEBUG)
        return;
    writeData(getFullData(msg, LOG_LEVEL_DEBUG, tags));
}
void Logging::info(String msg, LogTags *tags) {
    if (m_level > LOG_LEVEL_INFO)
        return;
    writeData(getFullData(msg, LOG_LEVEL_INFO, tags));
}
void Logging::warn(String msg, LogTags *tags) {
    if (m_level > LOG_LEVEL_WARNING)
        return;
    writeData(getFullData(msg, LOG_LEVEL_WARNING, tags));
}
void Logging::error(String msg, LogTags *tags) {
    writeData(getFullData(msg, LOG_LEVEL_ERROR, tags));
}

bool Logging::clear() {
    return m_storage->remove(LOGGING_FILE);
}

//////////////////// Private methods implementation
String LogTags::tagToString(logTag_t tag) {
    return String("[") + tag.name + ":" + tag.value + "]";
}

String Logging::getDateTime() {
    String s = "[dt:";

    if (m_dt != nullptr) {
        if (!m_dt->refresh())
            s += "ERR";
        else
            s += m_dt->toString();
    } else {
        s += "N/A";
    }
    return s + "]";
}

String Logging::getLogLevel(uint8_t level) {
    String s = "[level:";
    switch (level) {
    case LOG_LEVEL_DEBUG:
        s += "debug";
        break;
    case LOG_LEVEL_INFO:
        s += "info";
        break;
    case LOG_LEVEL_WARNING:
        s += "warn";
        break;
    case LOG_LEVEL_ERROR:
        s += "error";
        break;
    default:
        s += "N/A";
        break;
    }
    return s + "]";
}

String Logging::getFullData(String msg, uint8_t level) {
    return getDateTime() + getLogLevel(level) + "[msg:" + msg + "]";
}
String Logging::getFullData(String msg, uint8_t level, LogTags *tags) {
    return getFullData(msg, level) + tags->toString();
}

void Logging::writeData(String fullData) {
    String data = fullData + "\n";
    m_storage->appendFile(LOGGING_FILE, data.c_str());
}
