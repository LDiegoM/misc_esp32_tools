#include <internal/database/sqlite3.hpp>

//////////////////// Constructor
Sqlite3DB::Sqlite3DB(Storage *storage) {
    m_storage = storage;
}

//////////////////// Public methods implementation
bool Sqlite3DB::begin(String dbFileName) {
    lg->debug("Database::begin() init", __FILE__, __LINE__, lg->newTags()->add("db_file_name", dbFileName));
    if (!m_storage->exists(DB_PATH)) {
        lg->info("Database::begin() create directory", __FILE__, __LINE__,
            lg->newTags()->add("directory", DB_PATH)
        );
        m_storage->mkdir(DB_PATH);
    }

    m_dbPath = String(DB_PATH) + "/" + dbFileName;
    bool ok = sqlite3_initialize() == SQLITE_OK;
    if (!ok) {
        m_errMsg = "Database::begin() error: fail on sqlite3_initialize()";
        lg->error(m_errMsg, __FILE__, __LINE__);
    } else
        lg->debug("Database::begin() sqlite3 initialized", __FILE__, __LINE__);
    return ok;
}

String Sqlite3DB::getErrorMessage() {
    return m_errMsg;
}

String Sqlite3DB::getFilePath() {
    return m_dbPath;
}

bool Sqlite3DB::openDatabase() {
    m_errMsg = "";
    lg->debug("Database::openDatabase() init", __FILE__, __LINE__, lg->newTags()->add("db", m_dbPath));
    if (!existsDatabase()) {
        m_errMsg = "Database::openDatabase() db file not found";
        lg->warn(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("db", m_dbPath));
        return false;
    }
    
    if (m_db != nullptr)
        closeDatabase();
    
    String fullPath = String("/") + String(STORAGE_TYPE) + m_dbPath;
    if (sqlite3_open(fullPath.c_str(), &m_db) != SQLITE_OK) {
        m_errMsg = "Database::openDatabase() fail to open db: " + String(sqlite3_errmsg(m_db));
        lg->error(m_errMsg, __FILE__, __LINE__);
        closeDatabase();
        return false;
    }

    return true;
}

bool Sqlite3DB::closeDatabase() {
    m_errMsg = "";
    lg->debug("Database::closeDatabase() init", __FILE__, __LINE__, lg->newTags()->add("db", m_dbPath));

    if (m_db == nullptr)
        return true;

    bool ok = sqlite3_close(m_db) == SQLITE_OK;
    if (!ok) {
        m_errMsg = "Database::closeDatabase() fail to close db: " + String(sqlite3_errmsg(m_db));
        lg->warn(m_errMsg, __FILE__, __LINE__);
    }
    m_db = nullptr;
    return ok;
}

bool Sqlite3DB::existsDatabase() {
    return m_storage->exists(m_dbPath.c_str());
}

bool Sqlite3DB::existsTable(String table) {
    m_errMsg = "";
    String sql = "SELECT 1 FROM " + table + " WHERE 1=0";
    lg->debug("Database::existsTable() will execute a sentence", __FILE__, __LINE__, lg->newTags()->add("sql", sql));

    char *errMsg;
    bool ok = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg) == SQLITE_OK;
    if (!ok) {
        m_errMsg = String("Database::existsTable() error: ") + errMsg;
        lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
    }
    sqlite3_free(errMsg);
    return ok;
}

bool Sqlite3DB::createDatabase(String createSQL) {
    m_errMsg = "";
    lg->debug("Database::createDatabase() init", __FILE__, __LINE__,
        lg->newTags()->add("sql", createSQL)
    );
    if (!createSQL.startsWith("CREATE TABLE")) {
        m_errMsg = "Database::createDatabase() invalid call: sql sentence is not a CREATE TABLE statement";
        lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", createSQL));
        return false;
    }

    if (m_db != nullptr)
        closeDatabase();

    if (m_storage->exists(m_dbPath.c_str()))
        m_storage->remove(m_dbPath.c_str());

    String fullPath = String("/") + String(STORAGE_TYPE) + m_dbPath;
    if (sqlite3_open(fullPath.c_str(), &m_db) != SQLITE_OK) {
        m_errMsg = "Database::createDatabase() fail to open db: " + String(sqlite3_errmsg(m_db));
        lg->error(m_errMsg, __FILE__, __LINE__);
        closeDatabase();
        return false;
    }

    return execNonQuerySQL(createSQL);
}

bool Sqlite3DB::beginTransaction() {
    return execNonQuerySQL("BEGIN;");
}
bool Sqlite3DB::commitTransaction() {
    return execNonQuerySQL("COMMIT;");
}
bool Sqlite3DB::rollbackTransaction() {
    return execNonQuerySQL("ROLLBACK;");
}

bool Sqlite3DB::execNonQuerySQL(String sql) {
    m_errMsg = "";
    lg->debug("Database::execNonQuerySQL() will execute a sentence", __FILE__, __LINE__, lg->newTags()->add("sql", sql));
    char *errMsg;
    bool ok = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg) == SQLITE_OK;
    if (!ok) {
        m_errMsg = String("Database::execNonQuerySQL() error: ") + errMsg;
        lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
    }
    sqlite3_free(errMsg);
    return ok;
}

bool Sqlite3DB::execInsertWithAutoincrementSQL(String sql, int *rowID) {
    m_errMsg = "";
    lg->debug("Database::execInsertWithAutoincrementSQL() init", __FILE__, __LINE__,
        lg->newTags()->add("sql", sql)
    );
    if (!sql.startsWith("INSERT")) {
        m_errMsg = "Database::execInsertWithAutoincrementSQL() invalid call: sql sentence is not a INSERT statement";
        lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
        return false;
    }

    if (!beginTransaction()) {
        m_errMsg = "Database::execInsertWithAutoincrementSQL() fail to begin transaction: " + String(sqlite3_errmsg(m_db));
        lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
        return false;
    }

    sqlite3_stmt *res;
    const char *tail;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), sql.length(), &res, &tail) != SQLITE_OK) {
        rollbackTransaction();
        m_errMsg = "Database::execInsertWithAutoincrementSQL() error: " + String(sqlite3_errmsg(m_db));
        lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
        return false;
    }

    bool ok = sqlite3_step(res) == SQLITE_DONE;
    if (ok) {
        ok = getLastInsertedRowID(rowID);
    }

    sqlite3_finalize(res);

    if (ok)
        ok = commitTransaction();
    else
        rollbackTransaction();

    return ok;
}

int Sqlite3DB::getValueInt(String table, String field, String where, String orderBy) {
    m_errMsg = "";
    String sql = "SELECT " + field + " FROM " + table + " WHERE " + where;
    if (!orderBy.equals(""))
        sql += " ORDER BY " + orderBy;
    
    return getValueInt(sql);
}

int Sqlite3DB::getValueInt(String sql) {
    lg->debug("Database::getValueInt() will execute a sentence", __FILE__, __LINE__, lg->newTags()->add("sql", sql));

    sqlite3_stmt *res;
    const char *tail;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), sql.length(), &res, &tail) != SQLITE_OK) {
        m_errMsg = "Database::getValueInt() error: " + String(sqlite3_errmsg(m_db));
        lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
        return 0;
    }

    int retValue = 0;

    // This routine returns the value found at the first row, and first column.
    if (sqlite3_step(res) == SQLITE_ROW && sqlite3_column_count(res) > 0) {
        retValue = sqlite3_column_int(res, 0);
    } else {
        m_errMsg = "Database::getValueInt() error: not found";
        lg->error(m_errMsg, __FILE__, __LINE__);
    }

    sqlite3_finalize(res);

    return retValue;
}

float Sqlite3DB::getValueFloat(String table, String field, String where, String orderBy = "") {
    m_errMsg = "";
    String sql = "SELECT " + field + " FROM " + table + " WHERE " + where;
    if (!orderBy.equals(""))
        sql += " ORDER BY " + orderBy;
    lg->debug("Database::getValueFloat() will execute a sentence", __FILE__, __LINE__, lg->newTags()->add("sql", sql));

    sqlite3_stmt *res;
    const char *tail;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), sql.length(), &res, &tail) != SQLITE_OK) {
        m_errMsg = "Database::getValueFloat() error: " + String(sqlite3_errmsg(m_db));
        lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
        return 0;
    }

    float retValue = 0;

    // This routine returns the value found at the first row, and first column.
    if (sqlite3_step(res) == SQLITE_ROW && sqlite3_column_count(res) > 0) {
        retValue = sqlite3_column_double(res, 0);
    } else {
        m_errMsg = "Database::getValueFloat() error: not found";
        lg->error(m_errMsg, __FILE__, __LINE__);
    }

    sqlite3_finalize(res);

    return retValue;
}

bool Sqlite3DB::getResultsAsJSON(String sql, String *result) {
    m_errMsg = "";
    *result = "";
    if (!sql.startsWith("SELECT") && !sql.startsWith("WITH")) {
        m_errMsg = "Database::getResultsAsJSON() invalid call: SQL sentence can only be SELECT type";
        lg->warn(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
        return false;
    }
    lg->debug("Database::getResultsAsJSON() will execute a sentence", __FILE__, __LINE__, lg->newTags()->add("sql", sql));

    sqlite3_stmt *res;
    try {
        const char *tail;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), sql.length(), &res, &tail) != SQLITE_OK) {
            m_errMsg = String("Database::getResultsAsJSON() error: fail to fetch data: ") + sqlite3_errmsg(m_db);
            lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
            return false;
        }
    } catch (char *e) {
        m_errMsg = String("Database::getResultsAsJSON() error: catch on sqlite3_prepare_v2(): " + String(e));
        lg->error(m_errMsg, __FILE__, __LINE__);
        return false;
    }

    // FIXME: Create JSON as text, not using lib.
    *result = "{\"rows\":[";
    uint32_t recCount = 0;
    while (sqlite3_step(res) == SQLITE_ROW) {
        if (recCount > 0)
            *result += ",";
        *result += "{";
        int count = sqlite3_column_count(res);
        for (int i = 0; i<count; i++) {
            if (i > 0)
                *result += ",";
            *result += "\"" + String(sqlite3_column_name(res, i)) + "\":";
            switch (sqlite3_column_type(res, i)) {
            case SQLITE_TEXT:
                *result += "\"" + String((const char*) sqlite3_column_text(res, i)) + "\"";
                break;
            case SQLITE_INTEGER:
                *result += String(sqlite3_column_int(res, i));
                break;
            case SQLITE_FLOAT:
                *result += String(sqlite3_column_double(res, i));
                break;
            default:
                // Result column has an unknown type
                m_errMsg = String("Database::getResultsAsJSON() error: SQL response column [" + String(sqlite3_column_name(res, i)) + "] is of an unknown data type: ") + sqlite3_column_type(res, i);
                lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
                sqlite3_finalize(res);
                *result = "";
                return false;
            }
        }
        *result += "}";
        recCount++;
    }
    *result += "],\"count\":" + String(recCount) + "}";

    sqlite3_finalize(res);

    return true;
}

//////////////////// Private methods implementation
bool Sqlite3DB::getLastInsertedRowID(int *rowID) {
    m_errMsg = "";
    String sql = "SELECT last_insert_rowid();";

    lg->debug("Database::getLastInsertedRowID() will execute a sentence", __FILE__, __LINE__,
        lg->newTags()->add("sql", sql)
    );

    sqlite3_stmt *res;
    const char *tail;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), sql.length(), &res, &tail) != SQLITE_OK) {
        m_errMsg = "Database::getLastInsertedRowID() error: " + String(sqlite3_errmsg(m_db));
        lg->error(m_errMsg, __FILE__, __LINE__, lg->newTags()->add("sql", sql));
        return false;
    }

    bool ok = true;

    // This routine returns the value found at the first row, and first column.
    if (sqlite3_step(res) == SQLITE_ROW && sqlite3_column_count(res) > 0) {
        *rowID = sqlite3_column_int(res, 0);
    } else {
        m_errMsg = "Database::getLastInsertedRowID() error: not found";
        lg->error(m_errMsg, __FILE__, __LINE__);
        ok = false;
    }

    sqlite3_finalize(res);

    return ok;
}
