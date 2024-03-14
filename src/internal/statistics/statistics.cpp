#include <internal/statistics/statistics.hpp>

//////////////////// Constructor
Statistics::Statistics(Storage *storage, Sqlite3DB *db, DateTime *dt) {
    m_storage = storage;
    m_db = db;
    m_dt = dt;
}

//////////////////// Public methods implementation
bool Statistics::begin() {
    if (!m_db->begin(DB_FILE_NAME)) {
        return false;
    }

    if (!m_db->openDatabase() || !m_db->existsTable("events")) {
        return m_db->createDatabase(m_storage->readAll(CREATE_FILE));
    }

    int currentID = getCurrentDoorOpenID();
    lg->debug("Statistics::begin() current door event id", __FILE__, __LINE__, lg->newTags()->add("currentID", String(currentID)));
    if (currentID > 0) {
        m_currentDoorOpenEvent = currentID;
    }

    m_db->closeDatabase();

    return true;
}

String Statistics::getDbFilePath() {
    return m_db->getFilePath();
}

String Statistics::getDbSize() {
    if (m_storage == nullptr)
        return String("0 kb");

    return String(((float) m_storage->fileSize(getDbFilePath().c_str())) / 1024) + " kb";
}
bool Statistics::clear() {
    if (m_storage == nullptr)
        return false;

    m_db->closeDatabase();
    
    bool ok = m_storage->remove(getDbFilePath().c_str());
    if (ok)
        lg->info("statistics file was removed", __FILE__, __LINE__);
    else
        lg->error("fail to remove statistics file", __FILE__, __LINE__);

    return begin() && ok;
}

bool Statistics::addDoorOpening() {
    lg->debug("Statistics::addDoorOpening() init", __FILE__, __LINE__);

    if (m_currentDoorOpenEvent > 0) {
        lg->warn("fail to send door open to statistics db: door was already opened", __FILE__, __LINE__);
        return false;
    }

    if (!m_db->openDatabase()) {
        lg->error("Statistics::addDoorOpening() error", __FILE__, __LINE__, lg->newTags()->add("error", m_db->getErrorMessage()));
        return false;
    }

    String sql = "INSERT INTO events (time, event, warning) VALUES ('" + m_dt->toString() + "', 'door_open', 0);";
    int rowID;
    bool ok = m_db->execInsertWithAutoincrementSQL(sql, &rowID);
    if (ok) {
        lg->debug("Statistics::addDoorOpening() executed", __FILE__, __LINE__,
            lg->newTags()
                ->add("ok", String(ok))
                ->add("rowID", String(rowID))
        );
        m_currentDoorOpenEvent = rowID;
    } else
        lg->error("Statistics::addDoorOpening() error", __FILE__, __LINE__,
            lg->newTags()->add("error", m_db->getErrorMessage())
        );

    m_db->closeDatabase();
    return ok;
}

bool Statistics::doorOpenWarning() {
    lg->debug("Statistics::doorOpenWarning() init", __FILE__, __LINE__);

    if (m_currentDoorOpenEvent <= 0) {
        lg->error("fail to send door open warning to statistics db: no current event_id", __FILE__, __LINE__);
        return false;
    }

    if (!m_db->openDatabase()) {
        lg->error("Statistics::doorOpenWarning() error", __FILE__, __LINE__, lg->newTags()->add("error", m_db->getErrorMessage()));
        return false;
    }

    String sql = "UPDATE events SET warning = 1 WHERE id = " + String(m_currentDoorOpenEvent);
    bool ok = m_db->execNonQuerySQL(sql);
    if (ok)
        lg->debug("Statistics::doorOpenWarning() executed", __FILE__, __LINE__,
            lg->newTags()
                ->add("ok", String(ok))
        );
    else
        lg->error("Statistics::doorOpenWarning() error", __FILE__, __LINE__,
            lg->newTags()->add("error", m_db->getErrorMessage())
        );

    m_db->closeDatabase();
    return ok;
}

bool Statistics::closeDoor() {
    lg->debug("Statistics::closeDoor() init", __FILE__, __LINE__);

    if (m_currentDoorOpenEvent <= 0) {
        lg->error("fail to send door close to statistics db: no current event_id", __FILE__, __LINE__);
        return false;
    }

    if (!m_db->openDatabase()) {
        lg->error("Statistics::closeDoor() error", __FILE__, __LINE__, lg->newTags()->add("error", m_db->getErrorMessage()));
        return false;
    }

    String sql = "UPDATE events SET closed_time = '" + m_dt->toString() + "' WHERE id = " + String(m_currentDoorOpenEvent);
    bool ok = m_db->execNonQuerySQL(sql);
    if (ok) {
        m_currentDoorOpenEvent = -1;
        lg->debug("Statistics::closeDoor() executed", __FILE__, __LINE__,
            lg->newTags()
                ->add("ok", String(ok))
        );
    } else
        lg->error("Statistics::closeDoor() error", __FILE__, __LINE__,
            lg->newTags()->add("error", m_db->getErrorMessage())
        );

    m_db->closeDatabase();
    return ok;
}

last_opened_duration_t Statistics::getLastOpenedDuration() {
    lg->debug("Statistics::getLastOpenedDuration() init", __FILE__, __LINE__);

    last_opened_duration_t d;
    String sql = m_storage->readAll(VW_LAST_OPENED_DURATION);
    if (sql.equals("")) {
        lg->error("Statistics::getLastOpenedDuration() error: query sentence was not found", __FILE__, __LINE__, lg->newTags()->add("query_name", VW_LAST_OPENED_DURATION));
        return d;
    }

    if (!m_db->openDatabase()) {
        lg->error("Statistics::getLastOpenedDuration() error", __FILE__, __LINE__, lg->newTags()->add("error", m_db->getErrorMessage()));
        return d;
    }

    String jsonData = "";
    bool ok = m_db->getResultsAsJSON(sql, &jsonData);
    if (ok)
        lg->debug("Statistics::getLastOpenedDuration() executed", __FILE__, __LINE__, lg->newTags()
            ->add("ok", String(ok))
            ->add("json_data", jsonData)
        );
    else
        lg->error("Statistics::getLastOpenedDuration() error", __FILE__, __LINE__,
            lg->newTags()->add("error", m_db->getErrorMessage())
        );
    m_db->closeDatabase();

    StaticJsonDocument<1024> results;
    DeserializationError error = deserializeJson(results, jsonData);
    if (error) {
        lg->error("Statistics::getLastOpenedDuration() error: fail to parse response json", __FILE__, __LINE__, lg->newTags()
            ->add("error", String(error.c_str()))
        );
        return d;
    }
    JsonObject jsonObj = results.as<JsonObject>();
    if (jsonObj["rows"].size() > 0) {
        d.openedTime = jsonObj["rows"][0]["time"].as<String>();
        d.warnTriggered = jsonObj["rows"][0]["warning"].as<bool>();
        d.wasClosed = jsonObj["rows"][0]["was_closed"].as<bool>();
        d.openedDuration = jsonObj["rows"][0]["opened_duration"].as<String>();
    } else
        lg->info("Statistics::getLastOpenedDuration() no events where found in statistics db", __FILE__, __LINE__);

    jsonObj.clear();
    results.clear();
    jsonData.clear();
    return d;
}

// FIXME: This methos is not being used. Change it to return the opend time average of last 30 days.
std::vector<date_values_count_t> Statistics::getDoorOpeningsByMonth() {
    lg->debug("Statistics::getDoorOpeningsByMonth() init", __FILE__, __LINE__);

    std::vector<date_values_count_t> values;
    String sql = m_storage->readAll(VW_DOOR_OPENINGS_BY_MONTH);
    if (sql.equals("")) {
        lg->error("Statistics::getDoorOpeningsByMonth() error: query sentence was not found", __FILE__, __LINE__, lg->newTags()->add("query_name", VW_DOOR_OPENINGS_BY_MONTH));
        return values;
    }

    if (!m_db->openDatabase()) {
        lg->error("Statistics::getDoorOpeningsByMonth() error", __FILE__, __LINE__, lg->newTags()->add("error", m_db->getErrorMessage()));
        return values;
    }

    String jsonData = "";
    bool ok = m_db->getResultsAsJSON(sql, &jsonData);
    if (ok)
        lg->debug("Statistics::getDoorOpeningsByMonth() executed", __FILE__, __LINE__, lg->newTags()
            ->add("ok", String(ok))
            ->add("json_data", jsonData)
        );
    else
        lg->error("Statistics::getDoorOpeningsByMonth() error", __FILE__, __LINE__,
            lg->newTags()->add("error", m_db->getErrorMessage())
        );
    m_db->closeDatabase();

    StaticJsonDocument<1024> results;
    DeserializationError error = deserializeJson(results, jsonData);
    if (error) {
        lg->error("Statistics::getDoorOpeningsByMonth() error: fail to parse response json", __FILE__, __LINE__, lg->newTags()
            ->add("error", String(error.c_str()))
        );
        return values;
    }
    JsonObject jsonObj = results.as<JsonObject>();
    if (jsonObj["rows"].size() > 0) {
        for (size_t i = 0; i < jsonObj["rows"].size(); i++) {
            date_values_count_t v;
            v.period = jsonObj["rows"][i]["year_month"].as<String>();
            v.eventsCount = jsonObj["rows"][i]["events_qty"].as<int32_t>();
            v.warningsCount = jsonObj["rows"][i]["warning_qty"].as<int32_t>();

            values.push_back(v);
        }
    } else
        lg->info("Statistics::getDoorOpeningsByMonth() no events where found in statistics db", __FILE__, __LINE__);

    jsonObj.clear();
    results.clear();
    jsonData.clear();
    return values;
}

std::vector<date_values_count_t> Statistics::getStatistcsGraphLastPeriod(graphType period) {
    String vw = getGraphViewByPeriod(period);
    lg->debug("Statistics::getStatistcsGraphLastPeriod() init", __FILE__, __LINE__, lg->newTags()
        ->add("period", String(period))
        ->add("query_name", vw)
    );

    std::vector<date_values_count_t> values;
    String sql = m_storage->readAll(vw.c_str());
    if (sql.equals("")) {
        lg->error("Statistics::getStatistcsGraphLastPeriod() error: query sentence was not found", __FILE__, __LINE__, lg->newTags()->add("query_name", vw));
        return values;
    }

    if (!m_db->openDatabase()) {
        lg->error("Statistics::getStatistcsGraphLastPeriod() error", __FILE__, __LINE__, lg->newTags()->add("error", m_db->getErrorMessage()));
        return values;
    }

    String jsonData = "";
    bool ok = m_db->getResultsAsJSON(sql, &jsonData);
    if (ok)
        lg->debug("Statistics::getStatistcsGraphLastPeriod() executed", __FILE__, __LINE__, lg->newTags()
            ->add("ok", String(ok))
            ->add("json_data", jsonData)
        );
    else
        lg->error("Statistics::getStatistcsGraphLastPeriod() error", __FILE__, __LINE__,
            lg->newTags()->add("error", m_db->getErrorMessage())
        );
    m_db->closeDatabase();

    StaticJsonDocument<3000> results;
    DeserializationError error = deserializeJson(results, jsonData);
    if (error) {
        lg->error("Statistics::getStatistcsGraphLastPeriod() error: fail to parse response json", __FILE__, __LINE__, lg->newTags()
            ->add("error", String(error.c_str()))
        );
        return values;
    }
    JsonObject jsonObj = results.as<JsonObject>();
    if (jsonObj["rows"].size() > 0) {
        for (size_t i = 0; i < jsonObj["rows"].size(); i++) {
            date_values_count_t v;
            v.period = jsonObj["rows"][i]["period"].as<String>();
            v.eventsCount = jsonObj["rows"][i]["openings"].as<int32_t>();
            v.warningsCount = jsonObj["rows"][i]["warnings"].as<int32_t>();

            values.push_back(v);
        }
    } else
        lg->info("Statistics::getStatistcsGraphLastPeriod() no events where found in statistics db", __FILE__, __LINE__);

    jsonObj.clear();
    results.clear();
    jsonData.clear();
    return values;
}

//////////////////// Private methods implementation
int Statistics::getCurrentDoorOpenID() {
    String sql = m_storage->readAll(VW_CURRENT_EVENT_ID);
    if (sql.equals("")) {
        lg->error("Statistics::getCurrentDoorOpenID() error: query sentence was not found", __FILE__, __LINE__, lg->newTags()->add("query_name", VW_CURRENT_EVENT_ID));
        return 0;
    }

    return m_db->getValueInt(sql);
}

String Statistics::getGraphViewByPeriod(graphType period) {
    String vw = String(VW_GRAPH_LAST_PERIOD);
    switch (period) {
    case STATISTICS_GRAPH_LAST_DAY:
        vw.replace("{period}", "day");
        break;
    case STATISTICS_GRAPH_LAST_WEEK:
        vw.replace("{period}", "week");
        break;
    case STATISTICS_GRAPH_LAST_MONTH:
        vw.replace("{period}", "month");
        break;
    case STATISTICS_GRAPH_LAST_6_MONTHS:
        vw.replace("{period}", "6_months");
        break;
    default:
        return "";
        break;
    }

    return vw;
}