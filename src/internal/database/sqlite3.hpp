#ifndef sqlite3_db_h
#define sqlite3_db_h

#include <sqlite3.h>
#include <ArduinoJson.h>

#include <internal/core/storage.h>
#include <internal/core/date_time.h>
#include <internal/core/logging.h>

class Sqlite3DB {
    private:
        const char* DB_PATH = "/db";

        Storage *m_storage;
        String m_dbPath;
        sqlite3 *m_db = nullptr;
        const char *STORAGE_TYPE = "littlefs";

        String m_errMsg;

        bool getLastInsertedRowID(int *rowID);

    public:
        Sqlite3DB(Storage *storage);

        bool begin(String dbFileName);
        String getErrorMessage();
        String getFilePath();

        /*
         * The openDatabase() routine will try to open an existing database.
         * If file doesn't exists, will return false so client can call beginDatabase()
         * routine, passing as argument the full CREATE sentence.
        */
        bool openDatabase();
        bool closeDatabase();
        bool existsDatabase();
        bool existsTable(String table);
        bool createDatabase(String createSQL);

        bool beginTransaction();
        bool commitTransaction();
        bool rollbackTransaction();

        bool execNonQuerySQL(String sql);
        bool execInsertWithAutoincrementSQL(String sql, int *rowID);
        int getValueInt(String table, String field, String where, String orderBy);
        int getValueInt(String sql);
        float getValueFloat(String table, String field, String where, String orderBy);
        bool getResultsAsJSON(String sql, String *result);

};

#endif
