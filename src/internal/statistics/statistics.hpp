#ifndef statistics_h
#define statistics_h

#include <vector>
#include <ArduinoJson.h>

#include <internal/statistics/models.hpp>
#include <internal/database/sqlite3.hpp>
#include <internal/core/storage.h>
#include <internal/core/date_time.h>

enum graphType {
    STATISTICS_GRAPH_LAST_DAY = 0,
    STATISTICS_GRAPH_LAST_WEEK,
    STATISTICS_GRAPH_LAST_MONTH,
    STATISTICS_GRAPH_LAST_6_MONTHS
};

class Statistics {
    private:
        const char *DB_FILE_NAME = "statistics.db";
        const char *CREATE_FILE = "/db/create_statistics.sqlite";
        const char *VW_LAST_OPENED_DURATION   = "/db/vw_last_opened_duration.sqlite";
        const char* VW_CURRENT_EVENT_ID       = "/db/vw_current_event_id.sqlite";
        const char* VW_DOOR_OPENINGS_BY_MONTH = "/db/vw_door_openings_by_month.sqlite";
        const char* VW_GRAPH_LAST_PERIOD      = "/db/vw_stat_graph_{period}.sqlite";

        Storage *m_storage;
        Sqlite3DB *m_db;
        DateTime *m_dt;

        int m_currentDoorOpenEvent = -1;

        int getCurrentDoorOpenID();
        String getGraphViewByPeriod(graphType period);

    public:
        Statistics(Storage *storage, Sqlite3DB *db, DateTime *dt);

        bool begin();
        String getDbFilePath();
        String getDbSize();
        bool clear();

        bool addDoorOpening();
        bool doorOpenWarning();
        bool closeDoor();

        last_opened_duration_t getLastOpenedDuration();
        std::vector<date_values_count_t> getDoorOpeningsByMonth();

        std::vector<date_values_count_t> getStatistcsGraphLastPeriod(graphType period);

        /* These methos will be part of statistics class as part of the alarm_device
        uint32_t getTotalDoorOpeningsFrom(String date);
        uint32_t getTotalDoorOpeningsAt(String date);
        int getMaxOpenedTimeSinceDays(int days);
        int getMaxOpenedTimeSinceMonths(int days);*/
};

#endif
